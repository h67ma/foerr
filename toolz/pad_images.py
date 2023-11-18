# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2023 h67ma <szycikm@gmail.com>

import argparse
import os
from PIL import Image
from log import Log
from convert_data import pad_img_data


def pad_image(path_in: str, path_out: str, offset_left: int, offset_top: int):
	"""
	Saves a copy of the image at path_in in path_out with added transparent top & left offset.
	"""
	image = Image.open(path_in)
	width, height = image.size

	result = Image.new(image.mode, (width + offset_left, height + offset_top))
	result.paste(image, (offset_left, offset_top))
	result.save(path_out)


def pad_images(log: Log, path_in: str, path_out: str):
	for img_name, pad in pad_img_data.items():
		in_img_path = os.path.join(path_in, img_name)
		out_img_path = os.path.join(path_out, img_name)
		log.i("Adding padding to " + in_img_path)
		pad_image(in_img_path, out_img_path, pad[0], pad[1])


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for adding left & top padding to multiple images")
	parser.add_argument("-i", "--input", action="store", required=True, type=str, help=("Path to input dir containing images to process"))
	parser.add_argument("-o", "--output", action="store", required=True, type=str, help=("Output directory"))
	parser.add_argument("-l", "--log", action="store", type=int, default=3, help=("Log level"))
	args = parser.parse_args()

	log = Log(args.log)

	pad_images(log, args.input, args.output)
