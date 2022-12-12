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
