import argparse
import os
from PIL import Image
from convert_data import pad_img_data
from common import log_verbose


def pad_image(path_in: str, path_out: str, offset_left: int, offset_top: int):
	"""
	Saves a copy of the image at path_in in path_out with added transparent top & left offset.
	"""
	image = Image.open(path_in)
	width, height = image.size

	result = Image.new(image.mode, (width + offset_left, height + offset_top))
	result.paste(image, (offset_left, offset_top))
	result.save(path_out)


def pad_images(path_in: str, path_out: str):
	for img_name, pad in pad_img_data.items():
		in_img_path = os.path.join(path_in, img_name)
		out_img_path = os.path.join(path_out, img_name)
		log_verbose("Adding padding to " + in_img_path)
		pad_image(in_img_path, out_img_path, pad[0], pad[1])


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="A tool for adding left & top padding to multiple images")
	parser.add_argument("-i", "--input", action="store", required=True, type=str, help=("Path to input dir containing images to process"))
	parser.add_argument("-o", "--output", action="store", required=True, type=str, help=("Output directory"))
	args = parser.parse_args()

	pad_images(args.input, args.output)
