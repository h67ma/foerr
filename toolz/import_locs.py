# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2022-2023 h67ma <szycikm@gmail.com>

import os
import json
import argparse
from typing import List
from log import Log
from consts import *
from convert_data import *
from common import read_json, write_nicer_json, read_xml, nicify_texture_name
from xml.etree import ElementTree


def room_parse_options_node(out_room_node: object, room_node: ElementTree.Element, room_name: str, loc_liquid_type: str, loc_backwall_path: str, log: Log):
	"""
	Parses the <options> node present in <room> node alongside... well, everything else.
	Extract useful attributes and fill out_room_node.
	Return values used later on as a tuple.
	"""
	in_options_node = room_node.findall("options")

	if len(in_options_node) > 1:
		log.e(room_name + ": multiple <options> nodes declared, ignoring all")
		in_options_node = None
	elif len(in_options_node) != 0:
		in_options_node = in_options_node[0] # the only element
	else:
		in_options_node = None

	# liquids are handled differently than in Remains. Remains stores all liquids as the same symbol ('*'), and adds
	# information about its color in location or room settings ("wtip"). since there are only four possible liquids,
	# it makes more sense to define them as different materials and just place directly in cells. this approach also
	# makes it possible to have multiple types of liquids in a single room.
	room_liquid_symbol = liquid_type_map[0] # default (blue)

	# overwrite default liquid type with location-defined type
	if loc_liquid_type is not None:
		room_liquid_symbol = loc_liquid_type

	# "wtip" - room-wide liquid type
	if in_options_node is not None:
		in_liquid_type = in_options_node.attrib.get("wtip")
		if in_liquid_type is not None:
			in_liquid_type = int(in_liquid_type)
			if in_liquid_type in liquid_type_map:
				# overwrite default liquid type, or location-defined type, with room-defined type
				room_liquid_symbol = liquid_type_map[in_liquid_type]
			else:
				log.w(room_name + " has an unknown liquid type, ignoring")

	# "wlevel" - room-wide liquid height
	liquid_level = None
	if in_options_node is not None:
		in_liquid_level = in_options_node.attrib.get("wlevel")
		if in_liquid_level is not None:
			# "wlevel" is defined as "0 -> full room submerged". change it to "0 -> room not submerged" to be more intuitive
			liquid_level = ROOM_HEIGHT_WITH_BORDER - int(in_liquid_level)
			if liquid_level > 0:
				out_room_node[FOERR_JSON_KEY_LIQUID_LEVEL] = liquid_level
				# since we've placed a room-wide liquid level, we also need to define which liquid it is
				out_room_node[FOERR_JSON_KEY_LIQUID_SYMBOL] = room_liquid_symbol

	# "backform"
	# a few rooms in Remains define "backform" attribute, which means that backwall needs to be partially displayed.
	# there are two possible values:
	#   1 - display backwall only on left and right side, about 11 cells wide on each side (incl. border)
	#   2 - display backwall only on bottom side, about 9 cells high (incl. border)
	# instead of implementing this special case just for a handful of Rooms, let's instead use cell backgrounds to
	# achieve the desired effect, i.e. for cells included in the pattern which do not define background, we'll add
	# background set to backwall.
	room_backform = None
	if in_options_node is not None:
		in_backform = in_options_node.attrib.get("backform")
		if in_backform is not None:
			if in_backform in ["1", "2"]:
				log.v(room_name + " defines backform, adding background to some cells")
				room_backform = int(in_backform)
			else:
				log.w(room_name + " defines unknown backform, ignoring")

	# "backwall" - repeated background texture filling whole room
	room_backwall_defined = False
	backwall_value = None
	if in_options_node is not None:
		in_backwall = in_options_node.attrib.get("backwall")
		if in_backwall is not None:
			# "sky" as backwall value is a special case.
			# in ::drawBackWall() in Grafon.as room backwall is not drawn if "sky"
			# let's handle this differently than in Remains: let's not define backwall for location, as it can be
			# overwritten by room's backwall. let's just store backwall only in each room. if Remains location
			# defines backwall, just add it to every room which doesn't define its own backwall. additionally skip
			# "sky" as it causes room backwall to be ignored - with current approach we don't need it.
			room_backwall_defined = True
			if in_backwall != "sky" and in_backwall != "":
				# note: backwall="" happens only in Factory, and seems to make no sense. in this case backwall just
				# fallbacks to location backwall
				backwall_value = in_backwall

	if not room_backwall_defined and loc_backwall_path is not None:
		backwall_value = loc_backwall_path

	backwall_symbol = None
	if backwall_value is not None:
		backwall_value = nicify_texture_name(backwall_value)

		if room_backform is None:
			# write backwall normally
			out_room_node[FOERR_JSON_KEY_BACKWALL] = backwall_value
		else:
			# backform was specified, get character which we need to put instead of backwall into cells backgrounds
			# because so few rooms actually use backform, let's just hardcode symbols for these two textures here
			if backwall_value == "windows":
				backwall_symbol = 'u'
			elif backwall_value == "leaking":
				backwall_symbol = 'v'
			else:
				log.e(room_name + " no mapping found for " + backwall_value)

	# "lon" - set all background light objects in the room to on or off state
	if in_options_node is not None:
		in_lon = in_options_node.attrib.get("lon")
		if in_lon is not None:
			lon = int(in_lon)
			if lon not in [1, -1]:
				log.w(room_name + " declares unexpected \"lon\" value (" + in_lon + ")")

			# keep 1 and -1, it's a pretty intuitive representation without using enum
			out_room_node[FOERR_JSON_KEY_LIGHTS_ON] = lon

	# TODO parse remaining attributes:
	# "base"
	# "color"
	# "dark"
	# "desoff"
	# "noblack"
	# "nomap"
	# "rad"
	# "vis"
	# "wrad"
	# ...?

	return room_backform, backwall_symbol, room_liquid_symbol, liquid_level


def maybe_write_objs_node(objs_sorter, out_node, key):
	out_back_objs = []
	for layer_number in sorted(objs_sorter.keys()):
		for out_obj in objs_sorter[layer_number]:
			out_back_objs.append(out_obj)

	if len(out_back_objs) > 0:
		out_node[key] = out_back_objs


def translate_rooms(log: Log, output_id: str, input_filename: str, output_filename: str, loc_data, obj_data, pad_cnt: int, symbol_maps, mat_data) -> List[bool]:
	log.i("Translating " + input_filename + " to " + output_filename)

	input_root = read_xml(input_filename)
	if input_root is None:
		log.e("Error reading room xml, aborting")
		return

	max_cell_length = 0
	output_rooms = []

	is_unique_loc = False
	in_land_node = input_root.find("land")
	if in_land_node is not None and in_land_node.attrib.get("serial") == "1":
		log.i("Detected unique location")
		is_unique_loc = True
		if "start_room_x" not in loc_data or "start_room_y" not in loc_data:
			log.e("Starting room not specified, skipping location")
			return
		
		# note: "locz" is never specified in Remains, so we can assume z=0
		start_room_coords = (loc_data["start_room_x"], loc_data["start_room_y"], 0)
		start_room_found = False
	else:
		log.i("Detected grind location")

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
				log.w("Room " + room_name + ": missing coordinates, skipping room")
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

		##### options #####
		room_backform, backwall_symbol, room_liquid_symbol, liquid_level = room_parse_options_node(out_room_node, room_node, room_name, loc_liquid_type, loc_backwall_path, log)

		##### cells #####

		grid_rows = room_node.findall("a")
		if len(grid_rows) != ROOM_HEIGHT_WITH_BORDER:
			log.w("Room " + room_name + ": height is " + len(grid_rows) + ", should be " + ROOM_HEIGHT_WITH_BORDER + ", skipping room")
			continue

		out_cells = []

		for y, grid_row in enumerate(grid_rows):
			cells = grid_row.text.split(IN_DELIM)
			if len(cells) != ROOM_WIDTH_WITH_BORDER:
				log.w("Room " + room_name + " row " + y + ": width is " + len(cells) + ", should be " + ROOM_WIDTH_WITH_BORDER + ", skipping room")
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

				if is_unique_loc and (output_id in extra_cell_solids) and (room_coords in extra_cell_solids[output_id]) and ((x, y) in extra_cell_solids[output_id][room_coords]):
					# overwrite solid symbol
					overwrite_symbol = extra_cell_solids[output_id][room_coords][(x, y)]
					this_cell_symbols += overwrite_symbol
					if overwrite_symbol != SYMBOL_EMPTY and overwrite_symbol != SYMBOL_UNKNOWN:
						this_solid = True
				else:
					# normal parsing of solid symbol.
					# first symbol in input cell is always a solid
					if grid_elem[0] == SYMBOL_EMPTY:
						# if symbol is empty, encode empty too
						this_cell_symbols += SYMBOL_EMPTY
					else:
						# some solid defined in input
						out_symbol = symbol_maps[FOERR_JSON_KEY_SOLIDS].get(grid_elem[0], SYMBOL_UNKNOWN)
						if out_symbol == SYMBOL_UNKNOWN:
							log.w(err_prefix + "unknown solid: '" + grid_elem[0] + "'")
							this_cell_symbols += SYMBOL_UNKNOWN
						else:
							this_mat = mat_data[FOERR_JSON_KEY_SOLIDS].get(out_symbol)
							if this_mat is None:
								log.w(err_prefix + "unknown material for symbol ('" + out_symbol + "'), skipping symbol")
								this_cell_symbols += SYMBOL_UNKNOWN
							else:
								this_type = this_mat.get(FOERR_JSON_KEY_TYPE)
								if this_type != 1:
									log.w(err_prefix + "symbol in solid slot ('" + out_symbol + "') is not a solid, skipping symbol")
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
					log.v(err_prefix + "height flag defined, but no solid found, skipping height flag")
					this_skip_height_flags = True

				for character in grid_elem[1:]:
					if character in flag_part_height:
						if this_skip_height_flags:
							continue

						if this_part_height:
							log.w(err_prefix + "more than one height flag defined, skipping rest")
							continue
						this_part_height = True
						this_cell_symbols += character
						continue
					elif character == '*':
						if this_liquid:
							log.w(err_prefix + "more than one liquid defined, skipping rest")
							continue
						if this_solid and not this_has_any_part_height:
							# note: this appears a few times in Remains rooms and seems to be a mistake
							# it makes no sense to encode liquid on a full-sized cell.
							# Sewers/Canterlot are special cases as they have a map-wide liquid level.
							log.v(err_prefix + "liquid defined for full-sized cell containing a solid, skipping liquid")
							continue
						if liquid_level is not None and liquid_level >= ROOM_HEIGHT_WITH_BORDER - y:
							log.v(err_prefix + "liquid defined, but room-wide liquid level is already " + str(liquid_level) + ", skipping cell liquid")
							continue
						this_liquid = True
						this_cell_symbols += room_liquid_symbol # swap '*' for the correct liquid type
						continue

					out_symbol = symbol_maps[FOERR_JSON_KEY_OTHER].get(character, SYMBOL_UNKNOWN)
					if out_symbol == SYMBOL_UNKNOWN:
						log.w(err_prefix + "unknown symbol: " + character)
					else:
						this_mat = mat_data[FOERR_JSON_KEY_OTHER].get(out_symbol)
						if this_mat is None:
							log.w(err_prefix + "unknown material for symbol ('" + out_symbol + "'), skipping symbol")
							this_cell_symbols += SYMBOL_UNKNOWN
							continue

						this_type = this_mat.get(FOERR_JSON_KEY_TYPE)

						if this_type is None:
							log.w(err_prefix + "unknown symbol type, further sanity checks will be limited")
						else:
							if this_type == 2: # background
								if this_background:
									log.w(err_prefix + "more than one background defined, skipping rest")
									continue
								this_background = True
							elif this_type == 3: # ladder
								if this_ladder:
									log.w(err_prefix + "more than one ladder defined, skipping rest")
									continue
								if this_solid:
									log.w(err_prefix + "ladder found ('" + character + "'), but a solid is already defined for cell, skipping ladder")
									continue
								this_ladder = True
							elif this_type == 4: # platform
								if this_platform:
									log.w(err_prefix + "more than one platform defined, skipping rest")
									continue
								if this_solid:
									# note: this case happens a few times in Remains rooms. in this case a platform
									# is displayed on top of a solid, and the cell works like a solid would.
									# this makes no sense - if it behaves like a solid, then it should just display a
									# solid. displaying a platform instead might confuse the player.
									log.v(err_prefix + "platform found ('" + character + "'), but a solid is already defined for cell, skipping platform")
									continue
								if this_stairs:
									log.w(err_prefix + "platform found ('" + character + "'), but stairs are already defined for cell, skipping platform")
									continue
								this_platform = True
							elif this_type == 5: # stairs
								if this_stairs:
									log.w(err_prefix + "more than one stairs defined, skipping rest")
									continue
								if this_solid:
									log.w(err_prefix + "stairs found ('" + character + "'), but a solid is already defined for cell, skipping stairs")
									continue
								if this_platform:
									log.w(err_prefix + "stairs found ('" + character + "'), but platform is already defined for cell, skipping platform")
									continue
								this_stairs = True
							else:
								log.w(err_prefix + "unexpected symbol type (" + str(this_type) + "), skipping")
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
						log.w("Room " + room_name + " at " + str((x, y)) + ": cell is wider than requested pad size, trimming")
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
				log.w("Room" + room_name + ": back object " + str((back_id, back_x, back_y)) + " is missing attributes, skipping")
				continue

			if back_id not in obj_data:
				log.w("Room" + room_name + ": back object " + back_id, + " is missing from AllData, assuming layer 0")
				back_layer = 0
			else:
				back_layer = obj_data[back_id]

			variant = None
			if back_id in back_obj_variant_mapping:
				variant = back_obj_variant_mapping[back_id]
				back_id = variant[0]

			out_back_obj = {
				FOERR_JSON_KEY_COORDS: [int(back_x), int(back_y)],
				FOERR_JSON_KEY_ID: back_id
			}

			if variant is not None:
				out_back_obj[FOERR_JSON_KEY_VARIANT] = variant[1]

			if back_id in hole_ids:
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
				log.w("Room" + room_name + ": object " + str((obj_id, obj_x, obj_y)) + " is missing attributes, skipping")
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
		# also check if there's enough space to fit the player character (2x2 cells) on either side.
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
					this_room_cell_is_collider = out_room_geometry[y][0]
					if this_room_cell_is_collider != room_geometries[nearby_room_left][y][ROOM_WIDTH_WITH_BORDER - 1]:
						log.e("Room " + room_name + ": geometry validation failed at " + str((0, y)) + " - left edge mismatch")

					if not this_room_cell_is_collider:
						# edge cell is a passage - check both sides have enough space for player character
						if out_room_geometry[y][1]:
							log.e("Room " + room_name + ": geometry validation failed at " + str((1, y)) + " - insufficient space for the player")
						if room_geometries[nearby_room_left][y][ROOM_WIDTH_WITH_BORDER - 2]:
							log.e("Room " + room_name + ": geometry validation failed at " + str((1, y)) + " - insufficient space for the player in left room")

			if nearby_room_right in room_geometries:
				for y in range(ROOM_HEIGHT_WITH_BORDER):
					this_room_cell_is_collider = out_room_geometry[y][ROOM_WIDTH_WITH_BORDER - 1]
					if out_room_geometry[y][ROOM_WIDTH_WITH_BORDER - 1] != room_geometries[nearby_room_right][y][0]:
						log.e("Room " + room_name + ": geometry validation failed at " + str((ROOM_WIDTH_WITH_BORDER - 1, y)) + " - right edge mismatch")

					if not this_room_cell_is_collider:
						# edge cell is a passage - check both sides have enough space for player character
						if out_room_geometry[y][ROOM_WIDTH_WITH_BORDER - 2]:
							log.e("Room " + room_name + ": geometry validation failed at " + str((ROOM_WIDTH_WITH_BORDER - 2, y)) + " - insufficient space for the player")
						if room_geometries[nearby_room_right][y][1]:
							log.e("Room " + room_name + ": geometry validation failed at " + str((ROOM_WIDTH_WITH_BORDER - 2, y)) + " - insufficient space for the player in right room")

			if nearby_room_up in room_geometries:
				for x in range(ROOM_WIDTH_WITH_BORDER):
					this_room_cell_is_collider = out_room_geometry[0][x]
					if this_room_cell_is_collider != room_geometries[nearby_room_up][ROOM_HEIGHT_WITH_BORDER - 1][x]:
						log.e("Room " + room_name + ": geometry validation failed at " + str((x, 0)) + " - up edge mismatch")

					if not this_room_cell_is_collider:
						# edge cell is a passage - check both sides have enough space for player character
						if out_room_geometry[1][x]:
							log.e("Room " + room_name + ": geometry validation failed at " + str((x, 1)) + " - insufficient space for the player")
						if room_geometries[nearby_room_up][ROOM_HEIGHT_WITH_BORDER - 2][x]:
							log.e("Room " + room_name + ": geometry validation failed at " + str((x, 1)) + " - insufficient space for the player in upper room")

			if nearby_room_down in room_geometries:
				for x in range(ROOM_WIDTH_WITH_BORDER):
					this_room_cell_is_collider = out_room_geometry[ROOM_HEIGHT_WITH_BORDER - 1][x]
					if this_room_cell_is_collider != room_geometries[nearby_room_down][0][x]:
						log.e("Room " + room_name + ": geometry validation failed at " + str((x, ROOM_HEIGHT_WITH_BORDER - 1)) + " - down edge mismatch")

					if not this_room_cell_is_collider:
						# edge cell is a passage - check both sides have enough space for player character
						if out_room_geometry[ROOM_HEIGHT_WITH_BORDER - 2][x]:
							log.e("Room " + room_name + ": geometry validation failed at " + str((x, ROOM_HEIGHT_WITH_BORDER - 2)) + " - insufficient space for the player")
						if room_geometries[nearby_room_down][1][x]:
							log.e("Room " + room_name + ": geometry validation failed at " + str((x, ROOM_HEIGHT_WITH_BORDER - 2)) + " - insufficient space for the player in bottom room")

			room_geometries[room_coords] = out_room_geometry

	if is_unique_loc and not start_room_found:
		log.e("Start room specified but not found, skipping location")
		return

	output_root[FOERR_JSON_KEY_ROOMS] = output_rooms

	write_nicer_json(output_filename, output_root)

	return max_cell_length


def get_output_id(log: Log, input_basename: str):
	if input_basename in loc_names_map:
		input_basename = loc_names_map[input_basename] # translate loc name if possible
	else:
		msg = "Location name \"" + input_basename + "\" not translated, skipping"
		if input_basename in ["z_shablon", "rooms2"]:
			log.v(msg)
		else:
			log.w(msg)

		return None

	assert input_basename != ""
	return input_basename


def get_gamedata_data(log: Log, gamedata_path: str):
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

	input_root = read_xml(gamedata_path)
	if input_root is None:
		log.e("Error reading GameData, aborting")
		return None

	out_data = {}
	for loc_node in input_root.findall("land"):
		room_filename = loc_node.attrib.get("file")
		if room_filename is None:
			log.w("GameData <land/> is missing \"file\" attribute, skipping.")
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
					log.w("Background name \"" + background_full + "\" not translated")
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
					log.w(room_filename + " has an unknown liquid, ignoring")

		out_data[room_filename] = this_loc_data

	return out_data


def get_alldata_data(log: Log, alldata_path: str):
	"""
	Reads useful data from AllData.xml and converts it to a dictionary:
	{
		"obj_id": 3,	// layer number
		...
	}
	"""
	input_root = read_xml(alldata_path)
	if input_root is None:
		log.e("Error reading AllData, aborting")
		return None

	out_data = {}
	for back_node in input_root.findall("back"):
		back_id = back_node.attrib.get("id")
		if back_id is None:
			log.w("AllData <back/> is missing \"id\" attribute, skipping.")
			continue

		if back_id == "":
			# that's a super weird way of storing comments, what the hay
			continue

		layer = back_node.attrib.get("s")
		if layer is None:
			# for some reason some objs don't have layer attribute - defaults to 0
			log.v("AllData <back/> id=\"" + back_id + "\" is missing \"s\" attribute, assuming " + str(UNDEFINED_LAYER_NUMBER))
			layer = UNDEFINED_LAYER_NUMBER

		# note: we can also get obj size in cells from this node (x2, y2 attributes), if it's ever needed

		if back_id in out_data:
			log.w("Duplicate <back/> id=\"" + back_id + "\" in AllData, skipping")
			continue

		out_data[back_id] = int(layer)

	return out_data


def materials_to_map(log: Log, map_node, name: str):
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
			log.e(name +" symbols map contains non-unique values")
			return None
		out_map[symbol_legacy] = symbol
	return out_map


def load_verify_maps(log: Log, materials_path):
	mat_data = read_json(materials_path)
	if mat_data is None:
		return None

	to_process = [
		(FOERR_JSON_KEY_SOLIDS, "Solids"),
		(FOERR_JSON_KEY_OTHER, "Other"),
	]

	out_data = {}
	for map in to_process:
		if map[0] not in mat_data:
			log.e("\"" + map[1] + "\" map missing from materials.json")
			return None

		mapped = materials_to_map(log, mat_data[map[0]], map[1])
		if mapped is None:
			return None
		out_data[map[0]] = mapped

	# verify that location names map has unique values
	if len(loc_names_map) != len(set(loc_names_map.values())):
		log.e("Location names map contains non-unique values")
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
	parser.add_argument("-p", "--pad", action="store", type=int, help=("Pad each cell to this number of characters to align cells. Leave unspecified to disable padding."))
	parser.add_argument("-l", "--log", action="store", type=int, default=3, help=("Log level"))
	args = parser.parse_args()

	log = Log(args.log)

	symbol_maps, mat_data = load_verify_maps(log, args.materials)
	if symbol_maps is None or mat_data is None:
		exit()

	loc_data = get_gamedata_data(log, args.gamedata)
	if loc_data is None:
		exit()

	obj_data = get_alldata_data(log, args.alldata)
	if obj_data is None:
		exit()

	total_max_cell_length = 0

	if args.all:
		for filename in os.listdir(args.input):
			input_basename = get_loc_basename(filename)
			output_id = get_output_id(log, input_basename)

			if output_id is None:
				continue

			output_filename = output_id + ".json"

			if args.output is not None:
				output_path = os.path.join(args.output, output_filename)
			else:
				output_path = output_filename

			if input_basename not in loc_data:
				log.e("GameData does not contain data for " + input_basename + ", skipping")
			else:
				max_cell_length = translate_rooms(log, output_id, os.path.join(args.input, filename), output_path, loc_data[input_basename], obj_data, args.pad, symbol_maps, mat_data)
				if max_cell_length > total_max_cell_length:
					total_max_cell_length = max_cell_length

	else:
		input_basename = get_loc_basename(args.input)

		output_id = get_output_id(log, input_basename)

		if args.output is not None:
			output_filename = args.output
			assert output_filename != ""
		else:
			output_filename = output_id + ".json"

		if input_basename not in loc_data:
			log.e("GameData does not contain data for " + input_basename)
		else:
			total_max_cell_length = translate_rooms(log, output_id, args.input, output_filename, loc_data[input_basename], obj_data, args.pad, symbol_maps, mat_data)

	if args.pad is None:
		log.i("Max cell size was " + str(total_max_cell_length))
