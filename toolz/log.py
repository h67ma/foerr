# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2023 h67ma <szycikm@gmail.com>

from colorama import Fore, Style

class Log:
	def __init__(self, log_level: int):
		"""
		log_level:
			0 - don't log anything
			1 - log errors
			2 - log warnings
			3 - log informational messages
			4 - log verbose messages
			5 - log debug messages
		"""
		self.log_level = log_level


	@staticmethod
	def log_msg(level: str, msg: str, color):
		print(color + "[" + level + "] " + Style.RESET_ALL + msg)


	def d(self, msg):
		"""Log a debug message."""
		if self.log_level > 4:
			self.log_msg("DEBG", msg, Fore.LIGHTBLACK_EX)


	def v(self, msg):
		"""Log a verbose message."""
		if self.log_level > 3:
			self.log_msg("VERB", msg, Fore.BLUE)


	def i(self, msg):
		"""Log an informational message."""
		if self.log_level > 2:
			self.log_msg("INFO", msg, Fore.GREEN)


	def w(self, msg):
		"""Log a warning."""
		if self.log_level > 1:
			self.log_msg("WARN", msg, Fore.YELLOW)


	def e(self, msg):
		"""Log a critical error."""
		if self.log_level > 0:
			self.log_msg("ERRO", msg, Fore.RED)
