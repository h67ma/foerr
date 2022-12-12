import re
import json
from colorama import Fore, Style


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


def log_msg(level: str, msg: str, color):
	print(color + "[" + level + "] " + Style.RESET_ALL + msg)


def log_verbose(msg):
	log_msg("VERB", msg, Fore.LIGHTBLACK_EX)


def log_info(msg):
	log_msg("INFO", msg, Fore.GREEN)


def log_warn(msg):
	log_msg("WARN", msg, Fore.YELLOW)


def log_err(msg):
	log_msg("ERRO", msg, Fore.RED)


def write_rooms_json(output_filename: str, output_root):
	output_serialized = json.dumps(output_root, indent='\t')

	# YEET these ugly multiline coords
	output_serialized = re.sub(r"\"coords\": \[\s*(\d+),\s*(\d+),\s*(\d+)\s*\]", r""""coords": [\1, \2, \3]""", output_serialized)

	with open(output_filename, "w") as f:
		f.write(output_serialized)
