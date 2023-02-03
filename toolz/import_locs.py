import os
import re
import json
import argparse
from typing import List
import xml.etree.ElementTree as ET
from consts import *
from convert_data import *
from common import log_verbose, log_info, log_warn, log_err, sane_object_pairs_hook, write_nicer_json


reg_remove_end_variant = re.compile(r"(.+)(\d+)$")


def maybe_write_objs_node(objs_sorter, out_node, key):
	out_back_objs = []
	for layer_number in sorted(objs_sorter.keys()):
		for out_obj in objs_sorter[layer_number]:
			out_back_objs.append(out_obj)

	if len(out_back_objs) > 0:
		out_node[key] = out_back_objs


def maybe_separate_variant(obj_id: str):
	"""
	Detects if an object id wants to be a specific variant of an object which defines several variants.
	e.g. chole1 -> chole, variant 1
	e.g. chole -> random chole
	Returns a tuple of (real_object_id, variant_number) if variant was found, or None if variant was not found.
	"""
	search = reg_remove_end_variant.findall(obj_id)
	if len(search) != 1:
		return None
	
	obj_id = search[0][0]
	obj_variant = search[0][1]
	if obj_id in ["hole", "chole"]:
		return (obj_id, int(obj_variant) - 1) # also change index to be zero-based (more based than one-based)

	return None


def translate_rooms(input_filename: str, output_filename: str, loc_data, obj_data, pad_cnt: int, symbol_maps, mat_data) -> List[bool]:
	log_verbose("Translating " + input_filename + " to " + output_filename)

	try:
		input_tree = ET.parse(input_filename)
	except (FileNotFoundError, ET.ParseError) as ex:
		log_err(str(ex))
		return

	max_cell_length = 0
	output_rooms = []
	input_root = input_tree.getroot()

	is_unique_loc = False
	in_land_node = input_root.find("land")
	if in_land_node is not None and in_land_node.attrib.get("serial") == "1":
		log_verbose("Detected unique location")
		is_unique_loc = True
		if "start_room_x" not in loc_data or "start_room_y" not in loc_data:
			log_err("Starting room not specified, skipping location")
			return
		
		# note: "locz" is never specified in Remains, so we can assume z=0
		start_room_coords = (loc_data["start_room_x"], loc_data["start_room_y"], 0)
		start_room_found = False
	else:
		log_verbose("Detected grind location")

	output_root = {
		FOERR_JSON_KEY_API_VERSION: JSON_API_VERSION,
		FOERR_JSON_KEY_VERSION: 1,
	}

	loc_backwall_path = None
	if FOERR_JSON_KEY_BACKWALL in loc_data and loc_data[FOERR_JSON_KEY_BACKWALL] != "sky":
		loc_backwall_path = loc_data[FOERR_JSON_KEY_BACKWALL]

	loc_liquid_type = None
	if "liquid_symbol" in loc_data:
		loc_liquid_type = loc_data["liquid_symbol"]

	if FOERR_JSON_KEY_BACKGROUND_FULL in loc_data:
		output_root[FOERR_JSON_KEY_BACKGROUND_FULL] = loc_data[FOERR_JSON_KEY_BACKGROUND_FULL]

	room_geometries = {}

	for room_node in input_root.findall("room"):
		out_room_node = {}
		out_room_geometry = []

		##### basic infos #####

		room_name = "\"" + room_node.attrib.get("name", "?") + "\""

		if is_unique_loc:
			room_x = room_node.attrib.get("x")
			room_y = room_node.attrib.get("y")
			if room_x is None and room_y is None:
				log_warn("Room " + room_name + ": missing coordinates, skipping room")
				continue

			room_z = room_node.attrib.get("z", 0) # z is optional, assume 0 if not present

			room_x = int(room_x)
			room_y = int(room_y)
			room_z = int(room_z)

			room_coords = (room_x, room_y, room_z)

			out_room_node[FOERR_JSON_KEY_COORDS] = room_coords

			room_name += " " + str(room_coords)

			if room_coords == start_room_coords:
				out_room_node[FOERR_JSON_KEY_IS_START] = True
				start_room_found = True

		##### liquids #####

		# liquids are handled differently than in Remains. Remains stores all liquids as the same symbol ('*'), and adds
		# information about its color in location or room settings ("wtip"). since there are only four possible liquids,
		# it makes more sense to define them as different materials and just place directly in cells. this approach also
		# makes it possible to have multiple types of liquids in a single room.
		room_liquid_symbol = liquid_type_map[0] # default (blue)

		# overwrite default liquid type with location-defined type
		if loc_liquid_type is not None:
			room_liquid_symbol = loc_liquid_type

		in_options_node = room_node.find("options")
		if in_options_node is not None:
			liquid_type = in_options_node.attrib.get("wtip")
			if liquid_type is not None:
				liquid_type = int(liquid_type)
				if liquid_type in liquid_type_map:
					# overwrite default liquid type, or location-defined type, with room-defined type
					room_liquid_symbol = liquid_type_map[liquid_type]
				else:
					log_warn(room_name + " has an unknown liquid type, ignoring")

			liquid_level = in_options_node.attrib.get("wlevel")
			if liquid_level is not None:
				# "wlevel" is defined as "0 -> full room submerged". change it to "0 -> room not submerged" to be more intuitive
				liquid_level = ROOM_HEIGHT_WITH_BORDER - int(liquid_level)
				if liquid_level > 0:
					out_room_node[FOERR_JSON_KEY_LIQUID_LEVEL] = liquid_level
					# since we've placed a room-wide liquid level, we also need to define which liquid it is
					out_room_node[FOERR_JSON_KEY_LIQUID_SYMBOL] = room_liquid_symbol

		##### backform #####

		# a few rooms in Remains define "backform" attribute, which means that backwall needs to be partially displayed.
		# there are two possible values:
		#   1 - display backwall only on left and right side, about 11 cells wide on each side (incl. border)
		#   2 - display backwall only on bottom side, about 9 cells high (incl. border)
		# instead of implementing this special case just for a handful of Rooms, let's instead use cell backgrounds to
		# achieve the desired effect, i.e. for cells included in the pattern which do not define background, we'll add
		# background set to backwall.
		room_backform = None
		if in_options_node is not None:
			room_backform = in_options_node.attrib.get("backform")
			if room_backform is not None:
				if room_backform in ["1", "2"]:
					log_verbose(room_name + " defines backform, adding background to some cells")
					room_backform = int(room_backform)
				else:
					log_warn(room_name + " defines unknown backform, ignoring")

		##### room backwall #####

		room_backwall_defined = False
		backwall_value = None
		if in_options_node is not None: # skip backwall if backform was used
			in_backwall = in_options_node.attrib.get("backwall")
			if in_backwall is not None:
				# "sky" as backwall value is a special case.
				# in ::drawBackWall() in Grafon.as room backwall is not drawn if "sky"
				# let's handle this differently than in Remains: let's not define backwall for location, as it can be
				# overwritten by room's backwall. let's just store backwall only in each room. if Remains location
				# defines backwall, just add it to every room which doesn't define its own backwall. additionally skip
				# "sky" as it causes room backwall to be ignored - with current approach we don't need it.
				room_backwall_defined = True
				if in_backwall != "sky":
					backwall_value = in_backwall

		if not room_backwall_defined and loc_backwall_path is not None:
			backwall_value = loc_backwall_path

		backwall_symbol = None
		if backwall_value is not None:
			if room_backform is None:
				# write backwall normally
				out_room_node[FOERR_JSON_KEY_BACKWALL] = backwall_value
			else:
				# backform was specified, get character which we need to put instead of backwall into cells backgrounds
				# because so few rooms actually use backform, let's just hardcode symbols for these two textures here
				if backwall_value == "tWindows":
					backwall_symbol = 'u'
				elif backwall_value == "tLeaking":
					backwall_symbol = 'v'
				else:
					log_err(room_name + " no mapping found for " + backwall_value)

		##### cells #####

		grid_rows = room_node.findall("a")
		if len(grid_rows) != ROOM_HEIGHT_WITH_BORDER:
			log_warn("Room " + room_name + ": height is " + len(grid_rows) + ", should be " + ROOM_HEIGHT_WITH_BORDER + ", skipping room")
			continue

		out_cells = []

		for y, grid_row in enumerate(grid_rows):
			cells = grid_row.text.split(IN_DELIM)
			if len(cells) != ROOM_WIDTH_WITH_BORDER:
				log_warn("Room " + room_name + " row " + y + ": width is " + len(cells) + ", should be " + ROOM_WIDTH_WITH_BORDER + ", skipping room")
				continue

			out_cell_strip = []
			out_geometry_strip = []
			for x, grid_elem in enumerate(cells):
				# Remains cell is encoded as follows:
				# * first character defines solid type and is always present. it is replaced with '_' if cell doesn't
				#   contain a solid
				# * next comes a varied-length string of characters defining various elements:
				#   * background
				#   * platform
				#   * ladder
				#   * water
				#   * directive to lower solid height

				# RR cell was originally designed to have a constant size, to make the file align nicely. each character
				# in a cell would hold a designated type of symbol (background, solid/platform, stairs/ladder, flag).
				# however, with this approach a lot of space in file was wasted on empty symbols, while also making
				# encoding/decoding the cell more complicated (e.g. some pairs of symbols were combined into one symbol
				# to save cell width, while also preserving support for rare cell cases, e.g. part-height + solid +
				# + water). it was eventually decided to stick with Remains-style cell encoding, with added possibility
				# to pad cell with nothing ('_') up to a constant length to align cells, for debugging purposes.

				# in the end, RR cell uses two maps: one for the first symbol (solid), and second for rest of the
				# symbols following the first. this is necessary, because we want each symbol to be easily readable and
				# writeable by human, which leaves us with just 95 symbols (small letters, big letters, digits, and
				# basic interpunction). meanwhile, Remains defines around 70 symbols, which is pretty close to the
				# limit. in order to leave space for future additions (new solids/backgrounds/etc), we stick to using
				# two maps for now. however, if we were to switch to binary encoding, we'd have full 256 symbols
				# available, and we'd probably be fine with using a single map for all of them.

				err_prefix = "Room " + room_name + " at " + str((x, y)) + ": "

				this_cell_symbols = ""

				this_solid = False
				this_background = False
				this_ladder = False
				this_platform = False
				this_stairs = False
				this_liquid = False
				this_part_height = False

				# first symbol in input cell is always a solid
				if grid_elem[0] == SYMBOL_EMPTY:
					# if symbol is empty, encode empty too
					this_cell_symbols += SYMBOL_EMPTY
				else:
					# some solid defined in input
					out_symbol = symbol_maps[FOERR_JSON_KEY_SOLIDS].get(grid_elem[0], SYMBOL_UNKNOWN)
					if out_symbol == SYMBOL_UNKNOWN:
						log_warn(err_prefix + "unknown solid: '" + grid_elem[0] + "'")
						this_cell_symbols += SYMBOL_UNKNOWN
					else:
						this_mat = mat_data[FOERR_JSON_KEY_SOLIDS].get(out_symbol)
						if this_mat is None:
							log_warn(err_prefix + "unknown material for symbol ('" + out_symbol + "'), skipping symbol")
							this_cell_symbols += SYMBOL_UNKNOWN
						else:
							this_type = this_mat.get(FOERR_JSON_KEY_TYPE)
							if this_type != 1:
								log_warn(err_prefix + "symbol in solid slot ('" + out_symbol + "') is not a solid, skipping symbol")
								this_cell_symbols += SYMBOL_UNKNOWN
							else:
								this_solid = True
								this_cell_symbols += out_symbol

				# we need to check this before iterating over rest of the characters to cover detecting a case where
				# liquid is defined on a full-height cell containing a solid. we need to know if there is *any* part
				# height flag in order to not raise warning when liquid is defined before part-height flag.
				# we also need this to skip adding height flags in invalid cases (see below).
				this_has_any_part_height = False
				for character in grid_elem[1:]:
					if character in flag_part_height:
						this_has_any_part_height = True

				this_skip_height_flags = False
				if not this_solid and this_has_any_part_height:
					# this should also cover the cases where part-height is defined for platform/stairs/ladder, as
					# solid + platform/stairs/ladder will also trigger a warning.
					# note: this happens a fair number of times in Remains, and it appears to just display a normal
					# (full-height) background then.
					log_info(err_prefix + "height flag defined, but no solid found, skipping height flag")
					this_skip_height_flags = True

				for character in grid_elem[1:]:
					if character in flag_part_height:
						if this_skip_height_flags:
							continue

						if this_part_height:
							log_warn(err_prefix + "more than one height flag defined, skipping rest")
							continue
						this_part_height = True
						this_cell_symbols += character
						continue
					elif character == '*':
						if this_liquid:
							log_warn(err_prefix + "more than one liquid defined, skipping rest")
							continue
						if this_solid and not this_has_any_part_height:
							# note: this appears a few times in Remains rooms and seems to be a mistake
							# it makes no sense to encode liquid on a full-sized cell.
							# Sewers/Canterlot are special cases as they have a map-wide liquid level.
							log_info(err_prefix + "liquid defined for full-sized cell containing a solid, skipping liquid")
							continue
						if liquid_level is not None and liquid_level >= ROOM_HEIGHT_WITH_BORDER - y:
							log_info(err_prefix + "liquid defined, but room-wide liquid level is already " + str(liquid_level) + ", skipping cell liquid")
							continue
						this_liquid = True
						this_cell_symbols += room_liquid_symbol # swap '*' for the correct liquid type
						continue

					out_symbol = symbol_maps[FOERR_JSON_KEY_OTHER].get(character, SYMBOL_UNKNOWN)
					if out_symbol == SYMBOL_UNKNOWN:
						log_warn(err_prefix + "unknown symbol: " + character)
					else:
						this_mat = mat_data[FOERR_JSON_KEY_OTHER].get(out_symbol)
						if this_mat is None:
							log_warn(err_prefix + "unknown material for symbol ('" + out_symbol + "'), skipping symbol")
							this_cell_symbols += SYMBOL_UNKNOWN
							continue

						this_type = this_mat.get(FOERR_JSON_KEY_TYPE)

						if this_type is None:
							log_warn(err_prefix + "unknown symbol type, further sanity checks will be limited")
						else:
							if this_type == 2: # background
								if this_background:
									log_warn(err_prefix + "more than one background defined, skipping rest")
									continue
								this_background = True
							elif this_type == 3: # ladder
								if this_ladder:
									log_warn(err_prefix + "more than one ladder defined, skipping rest")
									continue
								if this_solid:
									log_warn(err_prefix + "ladder found ('" + character + "'), but a solid is already defined for cell, skipping ladder")
									continue
								this_ladder = True
							elif this_type == 4: # platform
								if this_platform:
									log_warn(err_prefix + "more than one platform defined, skipping rest")
									continue
								if this_solid:
									# note: this case happens a few times in Remains rooms. in this case a platform
									# is displayed on top of a solid, and the cell works like a solid would.
									# this makes no sense - if it behaves like a solid, then it should just display a
									# solid. displaying a platform instead might confuse the player.
									log_info(err_prefix + "platform found ('" + character + "'), but a solid is already defined for cell, skipping platform")
									continue
								if this_stairs:
									log_warn(err_prefix + "platform found ('" + character + "'), but stairs are already defined for cell, skipping platform")
									continue
								this_platform = True
							elif this_type == 5: # stairs
								if this_stairs:
									log_warn(err_prefix + "more than one stairs defined, skipping rest")
									continue
								if this_solid:
									log_warn(err_prefix + "stairs found ('" + character + "'), but a solid is already defined for cell, skipping stairs")
									continue
								if this_platform:
									log_warn(err_prefix + "stairs found ('" + character + "'), but platform is already defined for cell, skipping platform")
									continue
								this_stairs = True
							else:
								log_warn(err_prefix + "unexpected symbol type (" + str(this_type) + "), skipping")
								continue

					this_cell_symbols += out_symbol

				if room_backform is not None and not this_background and backwall_symbol is not None:
					# add backwall as cell background in specified places
					# we've already checked for unknown backform values when reading xml above
					if (room_backform == 1 and (x < 11 or x > 36)) or (room_backform == 2 and y > 15):
						# just add the symbol at the end as it's background
						this_cell_symbols += backwall_symbol

				if pad_cnt is not None:
					if len(this_cell_symbols) > pad_cnt:
						log_warn("Room " + room_name + " at " + str((x, y)) + ": cell is wider than requested pad size, trimming")
						this_cell_symbols = this_cell_symbols[:pad_cnt]
					else:
						for _ in range(len(this_cell_symbols), pad_cnt):
							this_cell_symbols += SYMBOL_EMPTY
				elif len(this_cell_symbols) > max_cell_length:
					max_cell_length = len(this_cell_symbols)

				out_cell_strip.append(this_cell_symbols)

				if is_unique_loc:
					out_geometry_strip.append(this_solid)

			out_cells.append(OUT_DELIM.join(out_cell_strip))

			if is_unique_loc:
				out_room_geometry.append(out_geometry_strip)

		out_room_node[FOERR_JSON_KEY_CELLS] = out_cells

		##### background objects #####

		# Remains defines a layer value ("s" attribute) for (almost all) back objs (undefined means layer 0).
		# because of this, the game has to sort objs according to their layer when loading the room.
		# RR handles this differently: instead of defining layer values, we assume that objs in Room are listed in
		# correct order. therefore, we need to sort objs by layer number when importing Rooms.
		# objects with higher layer values are drawn over objects with lower layer values.
		# objects with negative layer value are displayed behind backwall and background, so we need to store them in a
		# separate collection ("far_back_objs")

		back_objs_sorter = {}
		far_back_objs_sorter = {}
		back_holes_sorter = {} # sorting in this case probably doesn't matter, but let's do it anyway just in case
		for back_obj in room_node.findall("back"):
			back_id = back_obj.attrib.get("id")
			back_x = back_obj.attrib.get("x")
			back_y = back_obj.attrib.get("y")

			if back_id is None or back_x is None or back_y is None:
				log_warn("Room" + room_name + ": back object " + str((back_id, back_x, back_y)) + " is missing attributes, skipping")
				continue

			if back_id not in obj_data:
				log_warn("Room" + room_name + ": back object " + back_id, + " is missing from AllData, assuming layer 0")
				back_layer = 0
			else:
				back_layer = obj_data[back_id]

			maybe_separated_id = maybe_separate_variant(back_id)
			if maybe_separated_id is not None:
				back_id = maybe_separated_id[0]

			rr_back_id = "back_" + back_id

			out_back_obj = {
				FOERR_JSON_KEY_COORDS: [int(back_x), int(back_y)],
				FOERR_JSON_KEY_ID: rr_back_id
			}

			if maybe_separated_id is not None:
				out_back_obj[FOERR_JSON_KEY_VARIANT] = maybe_separated_id[1]

			if rr_back_id in hole_ids:
				if back_layer not in back_holes_sorter:
					back_holes_sorter[back_layer] = []
				back_holes_sorter[back_layer].append(out_back_obj)
			else:
				if back_layer >= 0:
					if back_layer not in back_objs_sorter:
						back_objs_sorter[back_layer] = []
					back_objs_sorter[back_layer].append(out_back_obj)
				else:
					if back_layer not in far_back_objs_sorter:
						far_back_objs_sorter[back_layer] = []
					far_back_objs_sorter[back_layer].append(out_back_obj)

		maybe_write_objs_node(back_objs_sorter, out_room_node, FOERR_JSON_KEY_BACK_OBJS)
		maybe_write_objs_node(far_back_objs_sorter, out_room_node, FOERR_JSON_KEY_FAR_BACK_OBJS)
		maybe_write_objs_node(back_holes_sorter, out_room_node, FOERR_JSON_KEY_BACK_HOLES)

		##### movable objects #####

		#out_objs = []
		for back_obj in room_node.findall("obj"):
			obj_id = back_obj.attrib.get("id")
			obj_x = back_obj.attrib.get("x")
			obj_y = back_obj.attrib.get("y")

			if obj_id is None or obj_x is None or obj_y is None:
				log_warn("Room" + room_name + ": object " + str((obj_id, obj_x, obj_y)) + " is missing attributes, skipping")
				continue

			obj_coords = [int(obj_x), int(obj_y)]

			if obj_id == "player":
				out_room_node[FOERR_JSON_KEY_SPAWN_COORDS] = obj_coords
				continue

			# TODO detect static, movable, enemies, npcs, area, etc, separate into proper collections

			#out_objs.append({
			#	FOERR_JSON_KEY_COORDS: obj_coords,
			#	FOERR_JSON_KEY_ID: obj_id
			#})

		#if len(out_objs) > 0:
		#	out_room_node[FOERR_JSON_KEY_OBJS] = out_objs

		output_rooms.append(out_room_node)

		##### geometry validation #####

		# validate room geometry by checking if connected room sides have the same layout of collider cells.
		# check all sides for every room. if there's no room on a given side, then validation also passes.
		# because we only check against already processed rooms, we might skip checking some sides, which will actually
		# be connected in the final location. that's ok though - they will be checked when the "missing" room is added.
		# basically each connection is checked only when both rooms on its sides are present. thanks to this, we will
		# check all connections, and we'll check each one exactly one time.
		# note: we're *not* checking connections in the Z axis, because walls don't matter in that case.
		# note: we're *not* validating grind locations

		if is_unique_loc:
			nearby_room_left = (room_x - 1, room_y, room_z)
			nearby_room_right = (room_x + 1, room_y, room_z)
			nearby_room_up = (room_x, room_y - 1, room_z)
			nearby_room_down = (room_x, room_y + 1, room_z)

			if nearby_room_left in room_geometries:
				for y in range(ROOM_HEIGHT_WITH_BORDER):
					if out_room_geometry[y][0] != room_geometries[nearby_room_left][y][ROOM_WIDTH_WITH_BORDER - 1]:
						log_err("Room " + room_name + ": geometry validation failed at " + str((y, 0)))

			if nearby_room_right in room_geometries:
				for y in range(ROOM_HEIGHT_WITH_BORDER):
					if out_room_geometry[y][ROOM_WIDTH_WITH_BORDER - 1] != room_geometries[nearby_room_right][y][0]:
						log_err("Room " + room_name + ": geometry validation failed at " + str((y, ROOM_WIDTH_WITH_BORDER - 1)))

			if nearby_room_up in room_geometries:
				for x in range(ROOM_WIDTH_WITH_BORDER):
					if out_room_geometry[0][x] != room_geometries[nearby_room_up][ROOM_HEIGHT_WITH_BORDER - 1][x]:
						log_err("Room " + room_name + ": geometry validation failed at " + str((0, x)))

			if nearby_room_down in room_geometries:
				for x in range(ROOM_WIDTH_WITH_BORDER):
					if out_room_geometry[ROOM_HEIGHT_WITH_BORDER - 1][x] != room_geometries[nearby_room_down][0][x]:
						log_err("Room " + room_name + ": geometry validation failed at " + str((ROOM_HEIGHT_WITH_BORDER - 1, x)))

			room_geometries[room_coords] = out_room_geometry

	if is_unique_loc and not start_room_found:
		log_err("Start room specified but not found, skipping location")
		return

	output_root[FOERR_JSON_KEY_ROOMS] = output_rooms

	write_nicer_json(output_filename, output_root)

	return max_cell_length


def get_output_filename(input_basename: str):
	if input_basename in loc_names_map:
		input_basename = loc_names_map[input_basename] # translate loc name if possible
	else:
		log_level = log_info if input_basename in ["z_shablon", "rooms2"] else log_warn
		log_level("Location name \"" + input_basename + "\" not translated, skipping")
		return None

	output_filename = input_basename + ".json"
	assert output_filename != ".json"
	
	return output_filename


def get_gamedata_data(gamedata_path: str):
	"""
	Reads useful data from GameData.xml and converts it to a dictionary:
	{
		"location_name": {
			"background_full": "path/to/background.png",
			"backwall": "texture_name",
			"start_room_x": x,
			"start_room_y": y,
			"liquid_symbol": '*'
		},
		...
	}
	"""

	try:
		input_tree = ET.parse(gamedata_path)
	except (FileNotFoundError, ET.ParseError) as ex:
		log_err(str(ex))
		return None

	input_root = input_tree.getroot()

	out_data = {}
	for loc_node in input_root.findall("land"):
		room_filename = loc_node.attrib.get("file")
		if room_filename is None:
			log_warn("GameData <land/> is missing \"file\" attribute, skipping.")
			continue

		this_loc_data = {}

		locx = loc_node.attrib.get("locx")
		locy = loc_node.attrib.get("locy")

		if locx is not None:
			this_loc_data["start_room_x"] = int(locx)
		if locy is not None:
			this_loc_data["start_room_y"] = int(locy)

		options_node = loc_node.find("options")
		if options_node is not None:
			background_full = options_node.attrib.get("fon")
			if background_full is not None:
				if background_full in bg_full_name_map:
					background_full = bg_full_name_map[background_full]
				else:
					log_warn("Background name \"" + background_full + "\" not translated")
				this_loc_data[FOERR_JSON_KEY_BACKGROUND_FULL] = background_full

			backwall = options_node.attrib.get("backwall")
			if backwall is not None:
				this_loc_data[FOERR_JSON_KEY_BACKWALL] = backwall

			liquid_type = options_node.attrib.get("wtip")
			if liquid_type is not None:
				liquid_type = int(liquid_type)
				if liquid_type in liquid_type_map:
					this_loc_data["liquid_symbol"] = liquid_type_map[liquid_type]
				else:
					log_warn(room_filename + " has an unknown liquid, ignoring")

		out_data[room_filename] = this_loc_data

	return out_data


def get_alldata_data(alldata_path: str):
	"""
	Reads useful data from AllData.xml and converts it to a dictionary:
	{
		"obj_id": 3,	// layer number
		...
	}
	"""
	try:
		input_tree = ET.parse(alldata_path)
	except (FileNotFoundError, ET.ParseError) as ex:
		log_err(str(ex))
		return None

	input_root = input_tree.getroot()
	out_data = {}
	for back_node in input_root.findall("back"):
		back_id = back_node.attrib.get("id")
		if back_id is None:
			log_warn("AllData <back/> is missing \"id\" attribute, skipping.")
			continue

		if back_id == "":
			# that's a super weird way of storing comments, what the hay
			continue

		layer = back_node.attrib.get("s")
		if layer is None:
			# for some reason some objs don't have layer attribute - defaults to 0
			log_info("AllData <back/> id=\"" + back_id + "\" is missing \"s\" attribute, assuming " + str(UNDEFINED_LAYER_NUMBER))
			layer = UNDEFINED_LAYER_NUMBER

		# note: we can also get obj size in cells from this node (x2, y2 attributes), if it's ever needed

		if back_id in out_data:
			log_warn("Duplicate <back/> id=\"" + back_id + "\" in AllData, skipping")
			continue

		out_data[back_id] = int(layer)

	return out_data


def materials_to_map(map_node, name: str):
	"""Reads materials node containing a dictionary of symbol: { material details }.
	Returns a dictionary mapping legacy symbol to translated symbol, based on data in materials file.
	If a material doesn't contain a legacy symbol, the normal (translated) symbol is used as key instead.
	"""
	out_map = {}
	for symbol, item in map_node.items():
		symbol_legacy = symbol
		if FOERR_JSON_KEY_SYMBOL_LEGACY in item:
			symbol_legacy = item[FOERR_JSON_KEY_SYMBOL_LEGACY]
		if symbol_legacy in out_map or symbol in out_map.values():
			log_err(name +" symbols map contains non-unique values")
			return None
		out_map[symbol_legacy] = symbol
	return out_map


def load_verify_maps(materials_path):
	try:
		with open(materials_path, "r") as f:
			mat_data = json.load(f, object_pairs_hook=sane_object_pairs_hook)
	except FileNotFoundError as ex:
		log_err(str(ex))
		return None

	to_process = [
		(FOERR_JSON_KEY_SOLIDS, "Solids"),
		(FOERR_JSON_KEY_OTHER, "Other"),
	]

	out_data = {}
	for map in to_process:
		if map[0] not in mat_data:
			log_err("\"" + map[1] + "\" map missing from materials.json")
			return None

		mapped = materials_to_map(mat_data[map[0]], map[1])
		if mapped is None:
			return None
		out_data[map[0]] = mapped

	# verify that location names map has unique values
	if len(loc_names_map) != len(set(loc_names_map.values())):
		log_err("Location names map contains non-unique values")
		return None

	return out_data, mat_data


def get_loc_basename(path: str):
	return os.path.splitext(os.path.basename(path))[0]


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for translating room data from Remains into FoERR format.")
	parser.add_argument("-i", "--input", action="store", required=True, type=str, help=("Path to xml file containing rooms stored in Remains format, or a directory containing them if -a is used"))
	parser.add_argument("-g", "--gamedata", action="store", required=True, type=str, help=("Path to GameData.xml (just remove beginning and end from GameData.as)"))
	parser.add_argument("-d", "--alldata", action="store", required=True, type=str, help=("Path to AllData.xml (just remove beginning and end from AllData.as)"))
	parser.add_argument("-m", "--materials", action="store", required=True, type=str, help=("Path to materials.json"))
	parser.add_argument("-o", "--output", action="store", required=False, type=str, help=("Path to output json file, or directory if -a is used"))
	parser.add_argument("-a", "--all", action="store_true", help=("Translate all files in input dir. If enabled, input/output should be a paths to directories, not files."))
	parser.add_argument("-p", "--pad", action="store", type=int, help=("Pad each cell to this number of characters to align cells"))
	args = parser.parse_args()

	symbol_maps, mat_data = load_verify_maps(args.materials)
	if symbol_maps is None or mat_data is None:
		exit()

	loc_data = get_gamedata_data(args.gamedata)
	if loc_data is None:
		exit()

	obj_data = get_alldata_data(args.alldata)
	if obj_data is None:
		exit()

	total_max_cell_length = 0

	if args.all:
		for filename in os.listdir(args.input):
			input_basename = get_loc_basename(filename)
			output_filename = get_output_filename(input_basename)
			if output_filename is None:
				continue

			if args.output is not None:
				output_path = os.path.join(args.output, output_filename)
			else:
				output_path = output_filename

			if input_basename not in loc_data:
				log_err("GameData does not contain data for " + input_basename + ", skipping")
			else:
				max_cell_length = translate_rooms(os.path.join(args.input, filename), output_path, loc_data[input_basename], obj_data, args.pad, symbol_maps, mat_data)
				if max_cell_length > total_max_cell_length:
					total_max_cell_length = max_cell_length

	else:
		input_basename = get_loc_basename(args.input)

		if args.output is not None:
			output_filename = args.output
			assert output_filename != ""
		else:
			output_filename = get_output_filename(input_basename)

		if input_basename not in loc_data:
			log_err("GameData does not contain data for " + input_basename)
		else:
			total_max_cell_length = translate_rooms(args.input, output_filename, loc_data[input_basename], obj_data, args.pad, symbol_maps, mat_data)

	if args.pad is None:
		log_verbose("Max cell size was " + str(total_max_cell_length))
