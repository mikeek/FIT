#!/usr/bin/python3

#JMP:xkozub03

import sys
import re
import Macro
import Config
from Config import exit_err

class Reader:
	index = 0;
	text = "";
	result = "";
	name_result = "";
	block_result = "";
	at_result = "";
	length = 0;
	in_block = 0;
	def next_char(self):
		self.index += 1;
	def act_char(self):
		return self.text[self.index];
	def read_mode(self):
		return _read_mode(self);
	def read_at(self):
		return _read_at(self);
	def read_name(self):
		return _read_name(self);
	def read_block(self):
		return _read_block(self);
	def read_def_block(self):
		return _read_def_block(self);
	def read_args(self, name):
		return _read_args(self, name);
	def __init__(self, input):
		self.text = input;
		self.length = len(input);
	def get_result(self):
		return self.result;

def _read_at(self):
	self.at_result = "";
	char = self.act_char();
	if re.match("(@|{|}|\$)", char):
		self.at_result += char;
		self.next_char();
		return False;
	elif re.match("[a-zA-Z_]", char):
		self.name_result = "@" + char;
		self.next_char();
		self.read_name();
		return True;
	else:
		exit_err("Syntax error (read_at)", 55);

def _read_name(self):
	while (self.index < self.length):
		if re.match("[0-9a-zA-Z_]", self.act_char()):
			self.name_result += self.act_char();
			self.next_char();
		else:
			return;

def _read_block(self):
	self.block_result = "";
	self.in_block += 1;
	while (self.index < self.length):
		char = self.act_char();
		if char == '{':
			self.in_block += 1;
			self.block_result += '{';
		elif char == '}':
			self.in_block -= 1;
			if self.in_block == 0:
				self.next_char();
				return;
			self.block_result += '}';
		elif char == '@':
			if re.match("(@|{|})", self.text[self.index + 1]):
				self.block_result += self.text[self.index + 1];
				self.next_char();
			else:
				self.block_result += char;
		else:
			self.block_result += char;
		self.next_char();
	exit_err("Syntax error ('{' without '}')", 55);

def _read_mode(self):
	while (self.index < self.length):
		char = self.act_char();
		if char == '@':
			self.next_char();
			if self.read_at():
#				got macro name - read arguments and expand it
				if self.name_result not in Macro.macro_list.keys():
					exit_err("Semantic error (macro '" + self.name_result + "' not found)", 56);
				name = self.name_result;
				self.read_args(name);
				to_join = Macro.macro_list[name].expand();
				self.text = to_join + self.text[self.index:];
				self.index = 0;
				self.length = len(self.text);
			else:
				self.result += self.at_result;
		elif char == '{':
			self.next_char();
			self.read_block();
			self.result += self.block_result;
		elif char == '}':
			exit_err("Syntax error ('}' without '{')", 55);
		elif char == '$':
			exit_err("Syntax error ('$')", 55);
		else:
			if not (Config.ignore_white and re.match("\s", char)):
				self.result += char;
			self.next_char();

def _read_args(self, name):
	cnt = Macro.macro_list[name].get_num_of_args();
	for i in range(0, cnt):
		if self.index >= self.length:
			exit_err("Semantic error (too few arguments for macro '" + name + "', " + str(i) + "/" + str(cnt) + ")", 56);
		if Config.ignore_white:
			while re.match("\s", self.act_char()):
				self.next_char();
				if self.index >= self.length:
					exit_err("Semantic error (too few arguments for macro '" + name + "', " + str(i) + "/" + str(cnt) + ")", 56);
		char = self.act_char();
		if Macro.macro_list[name].is_def and (i == 1 or i == 2) and char != '{':
			exit_err("def macro requires blocks as second and third arguments", 57);
		if Macro.macro_list[name].is_set and char != '{':
			exit_err("set macro requires block as argument", 57);
		if char == "@":
			self.next_char();
			if self.read_at():
				arg = self.name_result;
			else:
				arg = self.at_result;
		elif char == "{":
			self.next_char();
			if Macro.macro_list[name].is_def and i == 1:
				self.read_def_block();
			else:
				self.read_block();
			arg = self.block_result;
		else:
			arg = char;
			self.next_char();
		Macro.macro_list[name].set_next_arg(arg);

def _read_def_block(self):
	state = 0;
	self.block_result = "";
	while (self.index < self.length):
		char = self.act_char();
		if state == 0:
			if char == "$":
				state = 1;
			elif char == "}":
				self.next_char();
				return;
			elif not re.match("\s", char):
				exit_err("Wrong def argument list identifier", 55);
		elif state == 1:
			if not re.match("[a-zA-Z_]", char):
				exit_err("Wrong def argument list identifier", 55);
			state = 2;
		elif state == 2:
			if char == "$":
				state = 1;
			elif re.match("\s", char):
				state = 0;
			elif char == "}":
				self.next_char();
				return;
			elif not re.match("[0-9a-zA-Z_]", char):
				exit_err("Wrong def argument list identifier", 57);
		self.block_result += char;
		self.next_char();
	exit_err("Syntax error ('{' without '}')", 55);