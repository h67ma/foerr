import os
import json
import argparse
from log import Log
from common import sane_object_pairs_hook, write_nicer_json
from consts import FOERR_JSON_KEY_CELLS, FOERR_JSON_KEY_ROOMS, OUT_DELIM, SYMBOL_EMPTY

# let's be realistic
MAX_PAD_CNT = 10

def pad_loc(log: Log, target_filename: str, pad_cnt: int):
	if pad_cnt < 0 or pad_cnt > MAX_PAD_CNT:
		log.e("Invalid pad size: " + str(pad_cnt))
		return

	try:
		with open(target_filename, "r") as f:
			loc_root = json.load(f, object_pairs_hook=sane_object_pairs_hook)
	except FileNotFoundError as ex:
		log.e(str(ex))
		return

	err_prefix = "\"" + target_filename + "\": "

	if FOERR_JSON_KEY_ROOMS not in loc_root:
		log.e(err_prefix + FOERR_JSON_KEY_ROOMS + " not found")
		return

	for room_idx, room_node in enumerate(loc_root[FOERR_JSON_KEY_ROOMS]):
		if FOERR_JSON_KEY_CELLS not in room_node:
			log.e(err_prefix + "room " + str(room_idx) + ": " + FOERR_JSON_KEY_CELLS + " not found")
			return

		for row_idx, row in enumerate(room_node[FOERR_JSON_KEY_CELLS]):
			in_cells = row.split(OUT_DELIM)
			out_cells = []
			row_err_prefix = err_prefix + "room " + str(room_idx) + ": at row " + str(row_idx) + ": "
			for in_cell in in_cells:
				if len(in_cell) < 1:
					log.e(row_err_prefix + "empty cell")
					return

				this_solid = in_cell[0]
				this_other_stripped = in_cell[1:].strip(SYMBOL_EMPTY)

				out_cell = this_solid # copy solid, same in bolth cases

				if pad_cnt == 0: # 0 - do not pad
					out_cell += this_other_stripped # simple as
				else: # 1..MAX_PAD_CNT - pad
					if len(this_other_stripped) + 1 > pad_cnt:
						log.e(row_err_prefix + "cell is longer than requested pad size")
						return

					to_pad_cnt = pad_cnt - len(this_other_stripped) - 1
					out_cell += this_other_stripped + (SYMBOL_EMPTY * to_pad_cnt)

				out_cells.append(out_cell)

			# writeback
			room_node[FOERR_JSON_KEY_CELLS][row_idx] = OUT_DELIM.join(out_cells)

		# writeback
		loc_root[FOERR_JSON_KEY_ROOMS][room_idx] = room_node

	write_nicer_json(target_filename, loc_root)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Changes room format from variable cell width to fixed cell width with padding and vice versa in-place.")
	parser.add_argument("-t", "--target", action="store", required=True, type=str, help=("Path to a file containing RR rooms, or a directory containing them if -a is used"))
	parser.add_argument("-a", "--all", action="store_true", help=("Translate all files in input dir. If enabled, target should be a path to a directory, not a file."))
	parser.add_argument("-p", "--pad", action="store", type=int, required=True, help=("Pad each cell to this number of characters to align cells. 0 means no padding."))
	parser.add_argument("-l", "--log", action="store", type=int, default=3, help=("Log level"))
	args = parser.parse_args()

	log = Log(args.log)

	if args.all:
		for filename in os.listdir(args.target):
			pad_loc(log, os.path.join(args.target, filename), args.pad)
	else:
		pad_loc(log, args.target, args.pad)
