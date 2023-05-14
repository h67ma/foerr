import argparse
import shutil
import os
from log import Log
from common import *
from consts import *
from convert_data import *


def import_objs(log: Log, path_in: str, path_out: str, objs_out_path: str):
	"""
	Lists the directory containing exported object textures.

	Adds extra data (offset and blend mode, if needed) for each object based on convert_data.

	Generates objs.json with information about offset for each object and count of each texture type (except holes).

	Because the naming convention is strict, we don't have to store texture filenames - they can be deduced from object
	id, variant index, and texture type.
	"""
	grouped_objs = nicify_filenames_group_objs(log, path_in)

	out_meta_root = {
		FOERR_JSON_KEY_API_VERSION: JSON_API_VERSION,
		FOERR_JSON_KEY_VERSION: 1,
	}

	os.makedirs(path_out, exist_ok=True)
	out_objs_node = {}
	out_holes_node = {}

	for obj_id, obj_node in grouped_objs.items():
		out_obj_node = {}

		if obj_id not in obj_offsets:
			log.v("Object \"" + obj_id + "\" offset not found in convert_data, assuming (0, 0)")
		elif type(obj_offsets[obj_id]) is tuple:
			obj_offset = obj_offsets[obj_id]
			if obj_offset != (0, 0): # don't write zero offsets (note: comparing tuples here)
				out_obj_node[FOERR_JSON_KEY_OFFSET] = obj_offset

		if obj_id in objs_add_blend_flag:
			out_obj_node[FOERR_JSON_KEY_BLEND] = True

		# count the number of each texture type. non-present types are not included in output of
		# nicify_filenames_group_objs(), so they won't be included here as well (because their count is 0)
		for txt_type, txt_list in obj_node.items():
			if txt_type == TXT_TYPE_MAIN:
				out_obj_node[FOERR_JSON_KEY_MAIN_CNT] = len(txt_list)
			elif txt_type == TXT_TYPE_HOLE:
				# we skip TXT_TYPE_HOLE, as holes are stored in a separate list, and it is assumed that in case of holes
				# the number of main textures equals the number of hole textures
				pass
			elif txt_type == TXT_TYPE_LIGHT:
				out_obj_node[FOERR_JSON_KEY_LIGHT_CNT] = len(txt_list)
			else:
				log.w("Object \"" + obj_id + "\": unknown texture type (" + str(txt_type) + ")")

		if obj_id not in obj_copy_blacklist:
			for obj_type_node in obj_node.values():
				for txt_variant in obj_type_node:
					item_in_path = os.path.join(path_in, txt_variant[0] + ".png")
					item_out_path = os.path.join(path_out, txt_variant[1] + ".png")

					if os.path.exists(item_out_path):
						log.v(item_out_path + " already exists, skipping")
						continue

					shutil.copy(item_in_path, item_out_path)

		if txt_type in [TXT_TYPE_MAIN, TXT_TYPE_LIGHT]:
			out_objs_node[obj_id] = out_obj_node
		elif txt_type == TXT_TYPE_HOLE:
			out_holes_node[obj_id] = out_obj_node

	# sort output based on output filename
	out_meta_root[FOERR_JSON_KEY_BACK_OBJS] = {key: val for key, val in sorted(out_objs_node.items(), key = lambda item: item[0])}
	out_meta_root[FOERR_JSON_KEY_BACK_HOLES] = {key: val for key, val in sorted(out_holes_node.items(), key = lambda item: item[0])}
	write_nicer_json(objs_out_path, out_meta_root)
	log.w("Please manually fix trimmed and/or unpadded textures for: \"" + ("\", \"".join(obj_copy_blacklist) + "\"!"))


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for importing object data into objs.json")
	parser.add_argument("-i", "--input", action="store", required=True, type=str, help=("Path to input dir containing pngs exported from texture1.swf/sprites"))
	parser.add_argument("-o", "--output", action="store", required=True, type=str, help=("Output directory"))
	parser.add_argument("-b", "--objs", action="store", required=True, type=str, help=("Output path for objs.json"))
	parser.add_argument("-l", "--log", action="store", type=int, default=3, help=("Log level"))
	args = parser.parse_args()

	log = Log(args.log)

	import_objs(log, args.input, args.output, args.objs)
