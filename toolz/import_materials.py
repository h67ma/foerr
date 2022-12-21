import os
import json
import argparse
import xml.etree.ElementTree as ET
from consts import *
from common import log_verbose, log_info, log_warn, log_err
from convert_data import symbol_trans_map, extra_translations, missing_mats, mat_blacklist


def make_mat_translation_map(editoren_root):
	log_verbose("Preparing material translations from editor_en.xml")
	out_map = {}
	for in_obj_node in editoren_root.findall("obj"):
		pyccknn = in_obj_node.attrib.get("id")
		if pyccknn is None:
			log_warn("Translation is missing id, skipping")
			continue
		in_n_node = in_obj_node.find("n")
		if in_n_node is None or in_n_node.text == "":
			log_warn("\"" + pyccknn + "\" is missing translation, skipping")
			continue
		english = in_n_node.text
		out_map[pyccknn] = english
	return out_map


def import_materials(alldata_path: str, editoren_path: str, no_legacy: bool, output_filename: str):
	log_verbose("Importing materials")
	try:
		alldata_tree = ET.parse(alldata_path)
	except (FileNotFoundError, ET.ParseError) as ex:
		log_err(ex)
		return
	alldata_root = alldata_tree.getroot()

	try:
		editoren_tree = ET.parse(editoren_path)
	except (FileNotFoundError, ET.ParseError) as ex:
		log_err(ex)
		return
	editoren_root = editoren_tree.getroot()

	mat_name_map = make_mat_translation_map(editoren_root)

	materials_root = {
		FOERR_JSON_KEY_API_VERSION: JSON_API_VERSION,
		FOERR_JSON_KEY_SOLIDS: {},
		FOERR_JSON_KEY_OTHER: {},
	}

	imported_cnt = 0
	for mat in alldata_root.findall("mat"):
		out_mat = {}

		mat_name = mat.attrib.get("n")
		if mat_name is None:
			log_warn("Material is missing name, skipping")
			continue

		# escape the escape
		mat_name = mat_name.replace("\\\\", "\\")

		if mat_name in mat_name_map:
			mat_name = mat_name_map[mat_name] # thanks Alex for translations
		elif mat_name in extra_translations:
			mat_name = extra_translations[mat_name]

		# nicify filename
		mat_name = mat_name.replace("\\\\", "down") \
						   .replace("\\", "down") \
						   .replace("/", "up") \
						   .replace(" ", "_") \
						   .lower() \
						   .replace("steps", "stairs") \
						   .replace("beam", "platform")

		out_mat[FOERR_JSON_KEY_TEXTURE] = os.path.join(FOERR_PATH_CELL_TEXTURES, mat_name + ".png")

		mat_symbol = mat.attrib.get("id")
		if mat_symbol is None:
			log_warn("Material \"" + mat_name + "\" is missing symbol id")
			continue

		if mat_symbol in mat_blacklist:
			continue

		# translate symbol if translation is defined
		mat_symbol_translated = mat_symbol
		if mat_symbol in symbol_trans_map:
			mat_symbol_translated = symbol_trans_map[mat_symbol]

		if len(mat_symbol_translated) != 1:
			log_warn("Material \"" + mat_name + "\" symbol is not a single character, skipping material")
			continue

		# add legacy symbol if it's different from the translated one
		if not no_legacy and mat_symbol_translated != mat_symbol:
			out_mat[FOERR_JSON_KEY_SYMBOL_LEGACY] = mat_symbol

		mat_type = mat.attrib.get("ed")
		if mat_type is None:
			log_warn("Material \"" + mat_name + "\" is missing type")
			continue

		# RR symbol types:
		# 1 = solid
		# 2 = background
		# 3 = ladder
		# 4 = platform
		# 5 = stairs
		# 6 = liquid
		out_type = None
		if mat_type == "0" or mat_type == "1":
			# 0 is border solid
			# 1 is regular solid
			# we don't care which one is it
			out_type = FOERR_JSON_KEY_SOLIDS
			out_mat[FOERR_JSON_KEY_TYPE] = 1
		elif mat_type == "2":
			# 2 is background
			out_type = FOERR_JSON_KEY_OTHER
			out_mat[FOERR_JSON_KEY_TYPE] = 2
		elif mat_type == "3":
			# 3 is ladder
			out_type = FOERR_JSON_KEY_OTHER
			out_mat[FOERR_JSON_KEY_TYPE] = 3

			ladder_direction = mat.attrib.get("stair")
			if ladder_direction is None:
				log_warn("Ladder \"" + mat_name + "\" is missing direction, skipping")
				continue

			out_mat[FOERR_JSON_KEY_IS_RIGHT] = ladder_direction == "1"
		elif mat_type == "4":
			# 4 is stairs/platform
			out_type = FOERR_JSON_KEY_OTHER

			platform_value = mat.attrib.get("shelf")
			stairs_value = mat.attrib.get("diagon")

			if platform_value is not None:
				# platform
				out_mat[FOERR_JSON_KEY_TYPE] = 4
			elif stairs_value is not None:
				# stairs
				out_mat[FOERR_JSON_KEY_IS_RIGHT] = stairs_value == "1"
				out_mat[FOERR_JSON_KEY_TYPE] = 5
			else:
				log_warn("Material \"" + mat_name + "\" is neither platform nor stairs, or both at the same time, skipping")
		else:
			log_warn("Material \"" + mat_name + "\" has unknown type: \"" + mat_type + "\"")
			continue

		if mat_symbol_translated in materials_root[out_type]:
			log_warn("Material \"" + mat_name + "\" symbol is a duplicate, skipping material")
			continue
		materials_root[out_type][mat_symbol_translated] = out_mat
		imported_cnt += 1

	for key, node in missing_mats.items():
		if key in materials_root[FOERR_JSON_KEY_OTHER]:
			log_warn("Extra material \"" + key + "\" already present, skipping")
			continue
		materials_root[FOERR_JSON_KEY_OTHER][key] = node

	output_serialized = json.dumps(materials_root, indent='\t')
	with open(output_filename, "w") as f:
		f.write(output_serialized)
	log_info("Imported " + str(imported_cnt) + " materials")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for extracting material data from AllData to use in FoERR.")
	parser.add_argument("-d", "--alldata", action="store", required=True, type=str, help=("Path to AllData.xml (just remove beginning and end from AllData.as)"))
	parser.add_argument("-e", "--editoren", action="store", required=True, type=str, help=("Path to editor_en.xml"))
	parser.add_argument("-n", "--nolegacy", action="store_true", help=("Don't add legacy symbols"))
	parser.add_argument("-o", "--output", action="store", default="materials.json", type=str, help=("Output filename"))
	args = parser.parse_args()

	import_materials(args.alldata, args.editoren, args.nolegacy, args.output)
