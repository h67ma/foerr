import argparse
import shutil
import os
import re
from log import Log
from common import *
from consts import *
from convert_data import *
from common import BackObject


def back_obj_parse_alldata(back_node: ElementTree.Element) -> BackObject:
	"""
	Parses a <back/> node from AllData.
	Returns BackObject, or None if object should be skipped (invalid or not back object).
	"""
	back_obj = BackObject()

	back_obj.id = back_node.attrib.get("id")

	if back_obj.id is None:
		log.w("Background object missing \"id\" attribute, skipping")
		return None

	if len(back_obj.id) == 0:
		# apparently elements with empty ids are used as "comments" in the xml - skip them
		return None

	if back_node.attrib.get("tid") is not None or back_node.attrib.get("fr") is not None or back_obj.id in back_obj_missing_tid_fr:
		# objects with "fr" or "tid" defined are "named" variants of main object, e.g. "chole3" is a variant of "chole"
		# (tid='chole' fr='3'). these can be treated as duplicates. also some cases ("hole") require special manual
		# processing. therefore skip any such objects.
		# note: "hole1", "hole2", etc. are missing "tid" and "fr" for some reason; we also want to skip them.
		log.v("Background object \"%s\" has \"tid\" or \"fr\" defined, skipping" % back_obj.id)
		return None

	# initially assume every back object has main texture
	back_obj.has_main_txt = True

	back_alpha = back_node.attrib.get("alpha")
	if back_alpha is not None:
		back_obj.out_data[FOERR_JSON_KEY_ALPHA] = float(back_alpha)

	if back_node.attrib.get("blend") is not None and back_obj.id not in obj_back_blend_blacklist:
		# "blend" is only ever used only for 3 back objects:
		# "blood1" (= "multiply")
		# "blood2" (= "multiply")
		# "hole" (= "overlay")
		# for first two blend mode doesn't actually seem to do much, so let's ignore it.
		# let's just assume blend mode is a boolean
		back_obj.out_data[FOERR_JSON_KEY_BLEND] = True

	if back_node.attrib.get("er") is not None:
		# "er" present -> back object is a hole (i.e. has two textures: "*_t" and "*_e"
		back_obj.has_hole_txt = True

	if back_obj.id in obj_back_lights:
		# back object has "*_l" texture
		# note: we could partially find which objects are lights based on the "lon"/"loff" attributs, however they're
		# not set for all lights
		back_obj.has_light_txt = True

		if back_obj.id in obj_back_light_skip_txt_t:
			# some objects have an empty "*_t" texture or it's 1x1 px - skip it
			back_obj.has_main_txt = False

		# note: "lon" attribute always equals 1 if present, and it's only present if "loff" is present too, so it's
		# pretty much useless
		back_loff = back_node.attrib.get("loff")
		if back_loff is not None or back_obj.id in obj_back_light_loff_override:
			# note: some objects seem to be missing the "loff" attribute - hence check in obj_back_light_loff_override
			# "loff" is a one-based index of a variant which has only "*_t" texture and no "*_l" texture,
			# i.e. "light off". example:
			# loff = 3
			# variant 1: _t + _l
			# variant 2: _t + _l
			# variant 3: _t
			# let's give a different meaning to this key: light texture count
			# parse it to a number and decrease by 1 (so in above example loff - 1 = 2)
			# note: this key is later written to in back_objs_import(), but only if it's not defined

			if back_obj.id in obj_back_light_loff_override:
				# some objects have a clearly incorrect "loff" value - fix it
				back_loff = obj_back_light_loff_override[back_obj.id]

			back_obj.out_data[FOERR_JSON_KEY_LIGHT_CNT] = int(back_loff) - 1

	# remaining attributes:
	# "n" - seems to be a display string, perhaps needed for level editor. ignore it for now.
	# "s" - possibly layer number/draw order?
	# "mirror" - ?
	# "nope" - ?
	# "x2" - seems to be width (in cells), currently not useful
	# "y2" - seems to be height (in cells), currently not useful
	# ...?

	return back_obj


def back_obj_add_extra_data(back_obj: BackObject):
	"""
	Adds additional data to background object, not present in AllData.
	"""
	if back_obj.id not in back_obj_offsets:
		log.v("Background object \"" + back_obj.id + "\" offset not found in convert_data, assuming (0, 0)")
	elif type(back_obj_offsets[back_obj.id]) is tuple:
		obj_offset = back_obj_offsets[back_obj.id]
		if obj_offset != (0, 0): # don't write zero offsets (note: comparing tuples here and it somehow works)
			back_obj.out_data[FOERR_JSON_KEY_OFFSET] = obj_offset
	else:
		log.e("Invalid entry at back_obj_offsets[\"" + back_obj.id + "\"]")

	if back_obj.id in back_obj_lights_offsets:
		if type(back_obj_lights_offsets[back_obj.id]) is not tuple:
			log.e("Invalid entry at back_obj_lights_offsets[\"" + back_obj.id + "\"]")
			return

		obj_offset = back_obj_lights_offsets[back_obj.id]
		if obj_offset != (0, 0): # don't write zero offsets (note: comparing tuples here and it somehow works)
			back_obj.out_data[FOERR_JSON_KEY_OFFSET_LIGHT] = obj_offset


def back_obj_find_img_dir_names(txt_dirs: list[str], back_id: str) -> tuple[str, str, str]:
	"""
	Searches through txt_dirs list (should contain exported directory names).
	If a match against back_id is found, dir name is saved.
	This process applies to all three texture types: main, hole, light.
	Returns a tuple containing the three directory names (either can be None if it's not present).
	"""
	reg_main = re.compile("^DefineSprite_\d+_back_%s_t$" % back_id)
	reg_hole = re.compile("^DefineSprite_\d+_back_%s_e$" % back_id)
	reg_light = re.compile("^DefineSprite_\d+_back_%s_l$" % back_id)

	txt_main_name = None
	txt_hole_name = None
	txt_light_name = None

	for dir_name in txt_dirs:
		# we can assume the first match will be the only one, since we're doing a full match
		# and list items are assumed to be contents of a directory, so they're unique.

		if reg_main.fullmatch(dir_name) is not None:
			txt_main_name = dir_name
		elif reg_hole.fullmatch(dir_name) is not None:
			txt_hole_name = dir_name
		elif reg_light.fullmatch(dir_name) is not None:
			txt_light_name = dir_name

		# we could check if all three texture types are found and break search here,
		# but in practice this should never happen, as no object has all three texture types,
		# so checking is pointless.

	return txt_main_name, txt_hole_name, txt_light_name


def count_dir_elems(dir_path: str) -> int:
	return len(os.listdir(dir_path))


def back_obj_copy_imgs_to_output(obj_id: str, path_in: str, path_out: str, txt_cnt: str, filename_suffix: str):
	"""
	Copy background object images from directories exported from swf to output directory.
	Filenames are adjusted to have zero-based variant indexes (more based than one-based).
	"""
	if obj_id in back_obj_copy_blacklist:
		log.i("Background object \"%s\" present in copy blacklist, skipping. Please manually fix trimmed and/or unpadded textures with pad_images.py and copy to output dir." % obj_id)
		return

	for i in range(txt_cnt):
		item_path_in = os.path.join(path_in, str(i + 1) + ".png")
		item_path_out = os.path.join(path_out, "%s_%d%s.png" % (obj_id, i, filename_suffix))

		if not os.path.exists(item_path_in):
			log.e(main_in_path + " doesn't exist, make sure source files are named strictly")
			continue

		if os.path.exists(item_path_out):
			log.v(item_path_out + " already exists, skipping")
			continue

		shutil.copy(item_path_in, item_path_out)


def back_obj_light_copy_missing_last_main_txt(back_obj: BackObject, path_out_objs_back: str, main_txt_cnt: int):
	"""
	Some light back objects have "loff" defined, but main and light texture count are equal. To enable these objects to
	support light on/off states, duplicate the last main texture.
	Also increment main texture count in output data.
	If the object doesn't require this fix, do nothing.
	"""
	if back_obj.id not in obj_back_last_main_txt_missing:
		return

	if FOERR_JSON_KEY_MAIN_CNT not in back_obj.out_data:
		log.e("Background object \"%s\" missing main texture count, skipping last main texture duplication" % back_obj.id)
		return

	last_main_txt = os.path.join(path_out_objs_back, "%s_%d.png" % (back_obj.id, main_txt_cnt - 1))
	missing_main_txt = os.path.join(path_out_objs_back, "%s_%d.png" % (back_obj.id, main_txt_cnt))
	shutil.copy(last_main_txt, missing_main_txt)

	back_obj.out_data[FOERR_JSON_KEY_MAIN_CNT] += 1


def back_objs_import(log: Log, alldata: ElementTree.Element, path_in: str, path_res: str):
	"""
	Reads AllData <back/> nodes, extracts data from them.
	Fixes a whole bunch of inconsistencies and weirdness in AllData data.
	Finds the correct images for each object in the directory containing textures exported from swf.
	Adds extra data for each object based on convert_data if needed.
	Finally, generates objs.json with processed object data and copies images to output directory.

	Because the naming convention is strict, we don't have to store texture filenames - they can be deduced from object
	id, variant index, and texture type.
	"""
	log.i("Importing background objects")

	objs_json_path = os.path.join(path_res, FILE_OBJS_JSON)
	path_out_objs_back = os.path.join(path_res, PATH_OBJS_BACK)
	os.makedirs(path_out_objs_back, exist_ok=True)

	out_back_objs_node = {}
	out_back_holes_node = {}

	# contains dirs such as "DefineSprite_584_back_light7_t", "DefineSprite_329_visinencldoor", etc.
	# inside them are 1.png, 2.png, 3.png, etc.
	txt_dirs = os.listdir(path_in)

	parsed_back_objs = []

	for back_node in alldata.findall("back"):
		back_obj = back_obj_parse_alldata(back_node)
		if back_obj is None:
			continue
		parsed_back_objs.append(back_obj)

	parsed_back_objs.extend(obj_back_missing_parent_variants)

	for back_obj in parsed_back_objs:
		log.d("%s%s%s %s %s" % (
			"m" if back_obj.has_main_txt else "_",
			"h" if back_obj.has_hole_txt else "_",
			"l" if back_obj.has_light_txt else "_",
			back_obj.id,
			back_obj.out_data))

		txt_main_name, txt_hole_name, txt_light_name = back_obj_find_img_dir_names(txt_dirs, back_obj.id)

		# verify that texture files exist for each texture type

		if back_obj.has_main_txt and txt_main_name is None:
			log.e("Background object \"%s\" main texture missing, skipping" % back_obj.id)
			continue

		if back_obj.has_hole_txt and txt_hole_name is None:
			log.e("Background object \"%s\" hole texture missing, skipping" % back_obj.id)
			continue

		if back_obj.has_light_txt and txt_light_name is None:
			log.e("Background object \"%s\" light texture missing, skipping" % back_obj.id)
			continue

		back_obj_add_extra_data(back_obj)

		main_txt_cnt = 0
		hole_txt_cnt = 0
		light_txt_cnt = 0

		path_txt_main = None
		path_txt_hole = None
		path_txt_light = None

		if back_obj.has_main_txt:
			path_txt_main = os.path.join(path_in, txt_main_name)
			main_txt_cnt = count_dir_elems(path_txt_main)

		if back_obj.has_hole_txt:
			path_txt_hole = os.path.join(path_in, txt_hole_name)
			hole_txt_cnt = count_dir_elems(path_txt_hole)

		if back_obj.has_light_txt:
			path_txt_light = os.path.join(path_in, txt_light_name)
			light_txt_cnt = count_dir_elems(path_txt_light)

		if hole_txt_cnt > 0:
			# hole object

			if hole_txt_cnt != main_txt_cnt:
				log.e("Background hole object \"%s\" has different number of main and hole textures, skipping" % back_obj.id)
				continue

			if light_txt_cnt > 0:
				log.w("Background hole object \"%s\" declares having light texture, ignoring" % back_obj.id)

			back_obj.out_data[FOERR_JSON_KEY_MAIN_CNT] = main_txt_cnt
			# hole cnt is the same as main cnt, so no need to write it

			back_obj_copy_imgs_to_output(back_obj.id, path_txt_main, path_out_objs_back, main_txt_cnt, "")
			back_obj_copy_imgs_to_output(back_obj.id, path_txt_hole, path_out_objs_back, hole_txt_cnt, "_h")

			if back_obj.id in out_back_holes_node:
				log.w("Background hole object \"%s\" is a duplicate, overwriting previous" % back_obj.id)

			out_back_holes_node[back_obj.id] = back_obj.out_data
		else:
			# regular background object: main + light (both optional)

			if main_txt_cnt == 0 and light_txt_cnt == 0:
				log.e("Background object \"%s\" does not declare main or light textures, skipping" % back_obj.id)
				continue

			if main_txt_cnt > 0:
				# assume 0 if not defined
				back_obj.out_data[FOERR_JSON_KEY_MAIN_CNT] = main_txt_cnt
				back_obj_copy_imgs_to_output(back_obj.id, path_txt_main, path_out_objs_back, main_txt_cnt, "")

			if light_txt_cnt > 0:
				# assume 0 if not defined

				# FOERR_JSON_KEY_LIGHT_CNT may be already defined based on "loff" in parse_alldata_back_obj()
				if FOERR_JSON_KEY_LIGHT_CNT not in back_obj.out_data:
					# "loff" not defined - just put the light count in output
					back_obj.out_data[FOERR_JSON_KEY_LIGHT_CNT] = light_txt_cnt
				elif main_txt_cnt < 1:
					# "loff" defined, but main txt cnt is 0
					log.e("Background object \"%s\" declares light off index, but does not declare main textures, skipping" % back_obj.id)
					continue
				else:
					# "loff" defined, update local light cnt. this is done to skip copying any empty light textures
					# which are present in most light objects with "loff" defined.
					light_txt_cnt = back_obj.out_data[FOERR_JSON_KEY_LIGHT_CNT]

				back_obj_copy_imgs_to_output(back_obj.id, path_txt_light, path_out_objs_back, light_txt_cnt, "_l")

				back_obj_light_copy_missing_last_main_txt(back_obj, path_out_objs_back, main_txt_cnt)

			if back_obj.id in out_back_objs_node:
				log.w("Background object \"%s\" is a duplicate, overwriting previous" % back_obj.id)

			out_back_objs_node[back_obj.id] = back_obj.out_data

		log.d(str(back_obj.out_data))

	# output is sorted based on id
	out_meta_root = {
		FOERR_JSON_KEY_API_VERSION: JSON_API_VERSION,
		FOERR_JSON_KEY_VERSION: 1,
		FOERR_JSON_KEY_BACK_OBJS: {key: val for key, val in sorted(out_back_objs_node.items(), key = lambda item: item[0])},
		FOERR_JSON_KEY_BACK_HOLES: {key: val for key, val in sorted(out_back_holes_node.items(), key = lambda item: item[0])},
	}

	write_nicer_json(objs_json_path, out_meta_root)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for importing object data into objs.json")
	parser.add_argument("-i", "--input", action="store", required=True, type=str, help=("Path to input dir containing pngs exported from texture1.swf/sprites"))
	parser.add_argument("-d", "--alldata", action="store", required=True, type=str, help=("Path to AllData.xml (just remove beginning and end from AllData.as)"))
	parser.add_argument("-r", "--res", action="store", required=True, type=str, help=("Path to output directory (res)"))
	parser.add_argument("-l", "--log", action="store", type=int, default=3, help=("Log level"))
	args = parser.parse_args()

	log = Log(args.log)

	alldata_root = read_xml(args.alldata)
	if alldata_root is None:
		log.e("Error loading AllData, aborting")
		exit()

	back_objs_import(log, alldata_root, args.input, args.res)
