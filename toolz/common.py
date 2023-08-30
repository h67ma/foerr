import re
import os
import json
from xml.etree import ElementTree


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

def read_xml(path: str) -> ElementTree.Element:
	try:
		tree = ElementTree.parse(path)
	except (FileNotFoundError, ElementTree.ParseError) as ex:
		print(ex)
		return None
	return tree.getroot()
