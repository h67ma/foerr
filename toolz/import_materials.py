import argparse
import xml.etree.ElementTree as ET
from consts import *
from convert_data import *
from common import log_verbose, log_info, log_warn, log_err, write_nicer_json


def import_materials(alldata_path: str, no_legacy: bool, output_filename: str):
	log_verbose("Importing materials")
	try:
		alldata_tree = ET.parse(alldata_path)
	except (FileNotFoundError, ET.ParseError) as ex:
		log_err(ex)
		return
	alldata_root = alldata_tree.getroot()

	materials_root = {
		FOERR_JSON_KEY_API_VERSION: JSON_API_VERSION,
		FOERR_JSON_KEY_VERSION: 1,
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

		if mat_name in mat_translations_map:
			mat_name = mat_translations_map[mat_name]

		# get texture name if it's defined for this mat, else we'll fallback to material name
		texture_name = mat_name
		mat_main_node = mat.find("main")
		if mat_main_node is not None:
			texture_attrib = mat_main_node.attrib.get("tex")
			if texture_attrib is not None:
				texture_name = texture_attrib

		out_mat[FOERR_JSON_KEY_TEXTURE] = texture_name

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

			is_right = ladder_direction == "1"
			out_mat[FOERR_JSON_KEY_IS_RIGHT] = is_right
			out_mat[FOERR_JSON_KEY_OFFSET_LEFT] = LADDER_RIGHT_OFFSET_LEFT if is_right else LADDER_LEFT_OFFSET_LEFT
			out_mat[FOERR_JSON_KEY_TEXTURE_DELIM] = texture_name + "_delim"
			out_mat[FOERR_JSON_KEY_TEXTURE_DELIM_OFFSET] = LADDER_RIGHT_DELIM_OFFSET if is_right else LADDER_LEFT_DELIM_OFFSET
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

				if texture_name in STAIRS_OFFSETS_LEFT:
					out_mat[FOERR_JSON_KEY_OFFSET_LEFT] = STAIRS_OFFSETS_LEFT[texture_name]
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

	write_nicer_json(output_filename, materials_root)
	log_info("Imported " + str(imported_cnt) + " materials")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for extracting material data from AllData to use in FoERR.")
	parser.add_argument("-d", "--alldata", action="store", required=True, type=str, help=("Path to AllData.xml (just remove beginning and end from AllData.as)"))
	parser.add_argument("-n", "--nolegacy", action="store_true", help=("Don't add legacy symbols"))
	parser.add_argument("-o", "--output", action="store", default="materials.json", type=str, help=("Output filename"))
	args = parser.parse_args()

	import_materials(args.alldata, args.nolegacy, args.output)
