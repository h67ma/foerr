import argparse
import re
import os
from common import log_warn, log_err, nicify_filenames_group_objs, write_nicer_json


reg_find_transform = re.compile(r"<use[^\>]*matrix\((-?\d+\.\d+), (-?\d+\.\d+), (-?\d+\.\d+), (-?\d+\.\d+), (-?\d+\.\d+), (-?\d+\.\d+)\)")


def read_svg_offset(svg_path: str):
	"""
	Reads an svg file and tries to extract 2d translation from it. This is pretty terrible, but is currently the best
	known method of getting this data. The data is meant to be manually checked afterwards and copied to convert_data.py
	(obj_offsets).

	Returns a pair of ints on success.
	Returns None on fail.
	"""
	with open(svg_path, "r") as f:
		search = reg_find_transform.findall(f.read())
		if len(search) == 0:
			log_err("Transform data not found in \"" + svg_path + "\"")
			return None

		elif len(search) > 1:
			# seems like there are sometimes multiple elements in the sprite, thankfully the png contains all of them.
			# unfortunately we don't know which one we should consider as root :/
			# TODO we should read width/height of each "character" and somehow find the "max" transform for result png
			log_warn("Multiple transforms found in \"" + svg_path + "\", using the first one")

		if search[0][0] != "1.0" or search[0][1] != "0.0" or search[0][2] != "0.0" or search[0][3] != "1.0":
			log_warn("Funky transform found in \"" + svg_path + "\", ignoring scale/rotation/reflection/etc.")

		# who needs floats anyway?
		return (int(float(search[0][4])), int(float(search[0][5])))


def extract_svg_offsets(in_dir: str, out_path: str):
	"""
	Reads the svg files exported from texture1.swf/sprites and extracts 2d offsets from them. The data is considered
	"dirty" and requires manual processing to ensure that the offsets are correct. The manually-processed data should be
	added to obj_offsets (in convert_data.py).
	"""
	grouped_objs = nicify_filenames_group_objs(in_dir)

	out_meta_root = {}

	for obj_id, obj_node in grouped_objs.items():
		# check if all variants have the same offset
		first_offset = None
		all_offsets = {}
		all_offsets_same = True
		for obj_type_node in obj_node.values():
			for txt_variant in obj_type_node:
				txt_variant_offset = read_svg_offset(os.path.join(in_dir, txt_variant[0] + ".svg"))
				if txt_variant_offset is None:
					log_err("Transform data not found in \"" + txt_variant[0] + "\", please check offset manually")
					continue

				all_offsets[txt_variant[0]] = txt_variant_offset

				if first_offset is None:
					first_offset = txt_variant_offset
				elif first_offset != txt_variant_offset: # note: comparing *tuples* here, funny how this actually works
					all_offsets_same = False

		if all_offsets_same:
			# all offsets are the same - we store only the common offset
			if first_offset != (0, 0): # note: again comparing tuples
				out_meta_root[obj_id] = first_offset
		else:
			# there are differences between variant offsets - we store all of them for manual processing
			out_meta_root[obj_id] = all_offsets

	# sort output based on output filename
	out_meta_root = {key: val for key, val in sorted(out_meta_root.items(), key = lambda item: item[0])}
	write_nicer_json(out_path, out_meta_root)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for extracting padding data from svg files. The extracted transforms are meant to be tweaked manually before actual use.")
	parser.add_argument("-i", "--input", action="store", required=True, type=str, help=("Path to input dir containing svgs exported from texture1.swf/sprites"))
	parser.add_argument("-o", "--output", action="store", required=True, type=str, help=("Output filename"))
	args = parser.parse_args()

	extract_svg_offsets(args.input, args.output)
