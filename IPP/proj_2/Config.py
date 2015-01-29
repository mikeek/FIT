#!/usr/bin/python3

#JMP:xkozub03

import sys
import re

ignore_white = False;

def exit_err(type, code):
	sys.stderr.write(type + "!\n");
	exit(code);

class Config:
	help = False;
	redef_error = False;
	cmd_text = "";
	input = sys.stdin;
	output = sys.stdout;
	def parse_args(self, args):
		return conf_parse_args(self, args);
	def __init__(self):
		return;
	def get_help(self):
		return self.help;
	def get_redef_error(self):
		return self.redef_error;
	def get_cmd_text(self):
		return self.cmd_text;
	def get_input(self):
		return self.input;
	def get_output(self):
		return self.output;
	def open_files(self):
		try:
			if self._input_opt:
				self.input = open(self._input_name, 'r');
		except IOError:
			sys.stderr.write("Error when opening input file!\n");
			sys.exit(2);
		try:
			if self._output_opt:
				self.output = open(self._output_name, 'w');
		except IOError:
			self.input.close();
			sys.stderr.write("Error when opening output file!\n");
			sys.exit(3);
	def __del__(self):
		if not self.input.closed:
			self.input.close();
		if not self.output.closed:
			self.output.close();
	_input_name = "";
	_output_name = "";
	_input_opt = False;
	_output_opt = False;
	_help_opt = False;
	_redef_opt = False;
	_cmd_opt = False;

def multiple_exit(name):
	sys.stderr.write("Multiple occurences of '" + name + "' option!\n");
	sys.exit(1);

# pro maly rozsah projektu a vstupnich argumentu staci "hloupe" parsovani
def conf_parse_args(self, args):
	for opt in args[1:]:
		m = re.match("--input=(?P<input>.*)", opt);
		if m:
			if self._input_opt:
				multiple_exit("--input");
			self._input_opt = True;
			self._input_name = m.group("input");
			continue;
		m = re.match("--output=(?P<output>.*)", opt);
		if m:
			if self._output_opt:
				multiple_exit("--output");
			self._output_opt = True;
			self._output_name = m.group("output");
			continue;
		m = re.match("--cmd=(?P<text>.*)", opt);
		if m:
			if self._cmd_opt:
				multiple_exit("--cmd");
			self._cmd_opt = True;
			self.cmd_text = m.group("text");
			continue;
		if opt == "-r":
			if self._redef_opt:
				multiple_exit("-r");
			self._redef_opt = True;
			self.redef_error = True;
			continue;
		if opt == "--help":
			if self._help_opt:
				multiple_exit("--help");
			self._help_opt = True;
			self.help = True;
			if len(args) > 2:
				sys.stderr.write("Other arguments not allowed with '--help' option!\n");
				sys.exit(1);
			continue;
		sys.stderr.write("Unknown parameter '" + opt + "'!\n");
		sys.exit(1);
	self.open_files();