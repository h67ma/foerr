import os
import shutil
import pathlib
import argparse
from log import Log
from common import nicify_texture_name


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for renaming texture files to nicer names.")
	parser.add_argument("-i", "--input", action="store", type=str, required=True, help=("Input directory"))
	parser.add_argument("-l", "--log", action="store", type=int, default=3, help=("Log level"))
	args = parser.parse_args()

	log = Log(args.log)

	for filename in os.listdir(args.input):
		in_file_path = os.path.join(args.input, filename)

		if not os.path.isfile(in_file_path):
			continue

		in_file_path_obj = pathlib.Path(in_file_path)

		nicer_name = nicify_texture_name(in_file_path_obj.stem)

		if nicer_name == in_file_path_obj.stem:
			log.v(nicer_name + ": nicified name same as old one, skipping")
			continue

		out_file_path = os.path.join(args.input, nicer_name + in_file_path_obj.suffix)

		if os.path.exists(out_file_path):
			log.w(out_file_path + " already exists, skipping renaming")
			continue

		shutil.move(in_file_path, out_file_path)
		log.v("Renamed " + in_file_path_obj.stem + " to " + nicer_name)
