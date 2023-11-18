# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2023 h67ma <szycikm@gmail.com>

import argparse
import re
import os
from log import Log
from common import write_nicer_json


reg_find_transform = re.compile(r"<use[^\>]*matrix\((-?\d+\.\d+), (-?\d+\.\d+), (-?\d+\.\d+), (-?\d+\.\d+), (-?\d+\.\d+), (-?\d+\.\d+)\)")
reg_clean = re.compile(r"^DefineSprite_\d+")

TXT_TYPE_MAIN = 0
TXT_TYPE_HOLE = 1
TXT_TYPE_LIGHT = 2

texture_suffix_to_type = {
	"_t": TXT_TYPE_MAIN,
	"_e": TXT_TYPE_HOLE,
	"_l": TXT_TYPE_LIGHT,
}


def nicify_filenames_group_objs(log: Log, path_in: str, prefix: str):
	"""
	Generates more meaningful filenames (paths) for objects exported from texture1.swf/sprites, by removing
	"DefineSprite", random number, and "_t" (main variant), changing index to be zero-based, and un-directorizing
	the files.
	Output contains tuples containing old path (relative to path_in) in the first element, and new path
	(relative to new destination) in the second element. Both paths are without extensions.
	Tuples are grouped by object id and texture type (main/hole/light).

	Example output:
	{
		"back_hole5": {
			TXT_TYPE_MAIN: [
				("DefineSprite_1223_back_hole5_t/1", "back_hole5_0")
			],
			TXT_TYPE_HOLE: [
				("DefineSprite_1225_back_hole5_e/1", "back_hole5_0_e")
			]
		},
		"back_chains": {
			TXT_TYPE_MAIN: [
				("DefineSprite_858_back_chains_t/1", "back_chains_0"),
				("DefineSprite_858_back_chains_t/2", "back_chains_1"),
				("DefineSprite_858_back_chains_t/3", "back_chains_2")
			]
		},
		...
	}
	"""

	out_root = {}

	for img_id in os.listdir(path_in):
		img_id_clean = reg_clean.sub("", img_id)
		if len(img_id_clean) == 0:
			# some images don't have id, skip them for now
			log.v("Image \"" + img_id + "\" is missing id, skipping")
			continue

		# remove the leading underscore. we can't remove it via reg_clean, as the unnamed ids won't be detected then
		if img_id_clean[0] == '_':
			img_id_clean = img_id_clean[1:]

		if img_id_clean[:len(prefix)] != prefix:
			continue

		img_id_clean = img_id_clean[len(prefix):]

		# detect texture type
		img_id_suffix = ""
		texture_type = None
		if img_id_clean[-2:] in texture_suffix_to_type:
			texture_type = texture_suffix_to_type[img_id_clean[-2:]]

		# extract suffix which we'll add to id ("_t" we're ignoring)
		if texture_type in [TXT_TYPE_HOLE, TXT_TYPE_LIGHT]:
			img_id_suffix = img_id_clean[-2:]

		# remove suffix from id
		if texture_type is not None:
			img_id_clean = img_id_clean[:-2]
		else:
			texture_type = TXT_TYPE_MAIN # default

		img_idxes = os.listdir(os.path.join(path_in, img_id))

		if len(img_idxes) == 0:
			log.w("Image \"" + img_id + "\" has 0 images, skipping")
			continue

		out_obj_node = []

		for img_idx in img_idxes:
			orig_img_idx = os.path.splitext(img_idx)[0]
			try:
				based_img_idx = str(int(orig_img_idx) - 1) # convert to zero-based index
			except ValueError:
				log.e("Image \"" + img_id + "\": can't detect variant index, skipping")
				continue

			target_filename = img_id_clean + '_' + based_img_idx + img_id_suffix

			out_obj_node.append((
				os.path.join(img_id, orig_img_idx),
				target_filename
			))

		if len(out_obj_node) > 0:
			if img_id_clean not in out_root:
				out_root[img_id_clean] = {}
			elif texture_type in out_root[img_id_clean]:
				log.w("Image \"" + target_filename + "\" type already defined, skipping")
				continue

			out_root[img_id_clean][texture_type] = out_obj_node

	return out_root


def read_svg_offset(log: Log, svg_path: str):
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
			log.e("Transform data not found in \"" + svg_path + "\"")
			return None

		elif len(search) > 1:
			# seems like there are sometimes multiple elements in the sprite, thankfully the png contains all of them.
			# unfortunately we don't know which one we should consider as root :/
			# TODO we should read width/height of each "character" and somehow find the "max" transform for result png
			log.w("Multiple transforms found in \"" + svg_path + "\", using the first one")

		if search[0][0] != "1.0" or search[0][1] != "0.0" or search[0][2] != "0.0" or search[0][3] != "1.0":
			log.w("Funky transform found in \"" + svg_path + "\", ignoring scale/rotation/reflection/etc.")

		# who needs floats anyway?
		return (int(float(search[0][4])), int(float(search[0][5])))


def extract_group_svg_offsets(log: Log, in_dir: str, grouped_objs):
	out_node = {}

	for obj_id, obj_node in grouped_objs.items():
		# check if all variants have the same offset
		first_offset = None
		all_offsets = {}
		all_offsets_same = True
		for obj_type_node in obj_node.values():
			for txt_variant in obj_type_node:
				txt_variant_offset = read_svg_offset(log, os.path.join(in_dir, txt_variant[0] + ".svg"))
				if txt_variant_offset is None:
					log.e("Transform data not found in \"" + txt_variant[0] + "\", please check offset manually")
					continue

				all_offsets[txt_variant[0]] = txt_variant_offset

				if first_offset is None:
					first_offset = txt_variant_offset
				elif first_offset != txt_variant_offset: # note: comparing *tuples* here, funny how this actually works
					all_offsets_same = False

		if all_offsets_same:
			# all offsets are the same - we store only the common offset
			if first_offset != (0, 0): # note: again comparing tuples
				out_node[obj_id] = first_offset
		else:
			# there are differences between variant offsets - we store all of them for manual processing
			out_node[obj_id] = all_offsets

	return out_node


def extract_svg_offsets(log: Log, in_dir: str, out_path: str):
	"""
	Reads the svg files exported from texture1.swf/sprites and extracts 2d offsets from them. The data is considered
	"dirty" and requires manual processing to ensure that the offsets are correct. The manually-processed data should be
	added to obj_offsets (in convert_data.py).
	"""
	grouped_back_objs = nicify_filenames_group_objs(log, in_dir, "back_")
	grouped_objs = nicify_filenames_group_objs(log, in_dir, "vis")

	out_back_objs = extract_group_svg_offsets(log, in_dir, grouped_back_objs)
	out_objs = extract_group_svg_offsets(log, in_dir, grouped_objs)

	# sort outputs based on id
	out_meta_root = {
		"back_objs": {key: val for key, val in sorted(out_back_objs.items(), key = lambda item: item[0])},
		"objs": {key: val for key, val in sorted(out_objs.items(), key = lambda item: item[0])},
	}
	write_nicer_json(out_path, out_meta_root)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for extracting padding data from svg files. The extracted transforms are meant to be tweaked manually before actual use.")
	parser.add_argument("-i", "--input", action="store", required=True, type=str, help=("Path to input dir containing svgs exported from texture1.swf/sprites"))
	parser.add_argument("-o", "--output", action="store", required=True, type=str, help=("Output filename"))
	parser.add_argument("-l", "--log", action="store", type=int, default=3, help=("Log level"))
	args = parser.parse_args()

	log = Log(args.log)

	extract_svg_offsets(log, args.input, args.output)
