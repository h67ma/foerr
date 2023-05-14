import re
import os
import json
from log import Log
from convert_data import obj_txt_blacklist
from consts import TXT_TYPE_MAIN, TXT_TYPE_HOLE, TXT_TYPE_LIGHT


def sane_object_pairs_hook(ordered_pairs):
	"""Hook for loading json from string with support for detecting duplicate keys.
	If it's a *key*, then it's unique *by definition*! wtf
	Stolen from https://stackoverflow.com/questions/14902299/json-loads-allows-duplicate-keys-in-a-dictionary-overwriting-the-first-value"""
	d = {}
	for k, v in ordered_pairs:
		if k in d:
			raise ValueError("Duplicate key: %r" % (k,))
		d[k] = v
	return d


def write_nicer_json(output_filename: str, output_root):
	output_serialized = json.dumps(output_root, indent='\t')

	# YEET these ugly multiline coords
	output_serialized = re.sub(r"\[\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\s*\]", r"[\1, \2, \3]", output_serialized)
	output_serialized = re.sub(r"\[\s*(-?\d+),\s*(-?\d+)\s*\]", r"[\1, \2]", output_serialized)

	with open(output_filename, "w") as f:
		f.write(output_serialized)


reg_clean = re.compile(r"^DefineSprite_\d+")

texture_suffix_to_type = {
	"_t": TXT_TYPE_MAIN,
	"_e": TXT_TYPE_HOLE,
	"_l": TXT_TYPE_LIGHT,
}


def nicify_filenames_group_objs(log: Log, path_in: str):
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

	unnamed_idx = 0
	out_root = {}

	for img_id in sorted(os.listdir(path_in)): # sorted - we want the unnamed filenames to be in deterministic order
		img_id_clean = reg_clean.sub("", img_id)
		if len(img_id_clean) == 0:
			# some images don't have a name, TODO define correct names in convert_data and translate here
			img_id_clean = "unnamed" + str(unnamed_idx)
			log.w("Image \"" + img_id + "\" is missing id, using \"" + img_id_clean + "\"")
			unnamed_idx += 1

		# remove the leading underscore. we can't remove it via reg_clean, as the unnamed ids won't be detected then
		if img_id_clean[0] == '_':
			img_id_clean = img_id_clean[1:]

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

			if target_filename in obj_txt_blacklist:
				log.i("Image " + target_filename + " found in blacklist, skipping")
				continue

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
