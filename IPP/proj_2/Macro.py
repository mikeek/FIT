#!/usr/bin/python3

#JMP:xkozub03

import sys
import re
import Config
from Config import exit_err

macro_list = {};

def init_list(redef_opt):
	def_macro = Macro("@def");
	set_macro = Macro("@set");
	let_macro = Macro("@let");
	null_macro = Macro("@null");
	_def_macro = Macro("@__def__");
	_set_macro = Macro("@__set__");
	_let_macro = Macro("@__let__");
	
	def_macro.is_def   = True;
	set_macro.is_set   = True;
	let_macro.is_let   = True;
	null_macro.is_null = True;
	_def_macro.is_def  = True;
	_set_macro.is_set  = True;
	_let_macro.is_let  = True;
	
	def_macro.set_redef(redef_opt);
	_def_macro.set_redef(redef_opt);
	
	def_macro.add_arg("$frst");
	def_macro.add_arg("$scnd");
	def_macro.add_arg("$thrd");
	set_macro.add_arg("$frst");
	let_macro.add_arg("$frst");
	let_macro.add_arg("$scnd");
	_def_macro.add_arg("$frst");
	_def_macro.add_arg("$scnd");
	_def_macro.add_arg("$thrd");
	_set_macro.add_arg("$frst");
	_let_macro.add_arg("$frst");
	_let_macro.add_arg("$scnd");
	
	macro_list["@def"] = def_macro;
	macro_list["@set"] = set_macro;
	macro_list["@let"] = let_macro;
	macro_list["@null"] = null_macro;
	macro_list["@__def__"] = _def_macro;
	macro_list["@__set__"] = _set_macro;
	macro_list["@__let__"] = _let_macro;

class Macro:
	name = "";
	body = "";
	args = {};
	args_ord_name = [];
	args_cnt = 0;
	args_order = 0;
	is_def = False;
	is_set = False;
	is_let = False;
	is_null = False;
	redef = True;
	def __init__(self, name):
		self.name = name;
		self.body = "";
		self.args = {};
		self.args_ord_name = [];
		self.args_cnt = 0;
		self.args_order = 0;
		self.is_def = False;
		self.is_set = False;
		self.is_let = False;
		self.is_null = False;
		self.redef = True;
		return;
	def set_redef(self, redef):
		self.redef = redef;
	def get_name(self):
		return self.name;
	def get_num_of_args(self):
		return self.args_cnt;
	def add_arg(self, name):
		if name in self.args.keys():
			#redefinice argumentu;
			exit_err("Semantic error (argument redefinition - '" + name + "')", 56);
		self.args[name] = '';
		self.args_ord_name.append(name);
		self.args_cnt += 1;
	def set_next_arg(self, value):
		if self.args_order == self.args_cnt:
			#prilis mnoho parametru
			sys.stderr.write("Syntax error\n");
			sys.exit(56);
		if self.is_def and self.args_order == 0 and value[0] != '@':
			exit_err("Macro name expected ('" + value + "' given)", 57);
		self.args[self.args_ord_name[self.args_order]] = value;
		self.args_order += 1;
	def set_body(self, body):
		self.body = body;
	def expand(self):
		return _expand(self);
	def expand_def(self):
		return _expand_def(self);
	def expand_set(self):
		return _expand_set(self);
	def expand_let(self):
		return _expand_let(self);
	def expand_null(self):
		return _expand_null(self);

def _expand(self):
	if self.args_order != self.args_cnt:
		sys.stderr.write("Syntax error\n");
		exit(56);
	self.args_order = 0;
	if self.is_def:
		return self.expand_def();
	if self.is_set:
		return self.expand_set();
	if self.is_null:
		return self.expand_null();
	if self.is_let:
		return self.expand_let();
	exp_body = self.body;
	m = re.findall("((^|[^\$]*?)(\$[a-zA-Z_][a-zA-Z_0-9]*)(\s|\$|$|[^a-zA-Z_0-9]))", exp_body);
	for rex in m:
		if rex[2] in self.args.keys():
			exp_body = exp_body.replace(rex[0], rex[1] + self.args[rex[2]] + rex[3]);
	return exp_body;

def _expand_def(self):
	name = self.args[self.args_ord_name[0]];
	arg_list = self.args[self.args_ord_name[1]];
	def_body = self.args[self.args_ord_name[2]];
	new_macro = Macro(name);
	if name == "@__def__" or name == "@__let__" or name == "@__set__":
		exit_err("Redef __macro__ error", 57);
	if name == "@null":
		return "";
	if self.redef and name in macro_list:
		exit_err("Redef -r macro error", 57);
	m = re.findall("\$[a-zA-Z_][a-zA-Z_0-9]*", arg_list);
	for rex in m:
		new_macro.add_arg(rex);
	new_macro.set_body(def_body);
	macro_list[name] = new_macro;
	return "";

def _expand_set(self):
	self.body = "";
	set = self.args[self.args_ord_name[0]];
	global ignore_white;
	if set == "-INPUT_SPACES":
		Config.ignore_white = True;
	elif set == "+INPUT_SPACES":
		Config.ignore_white = False;
	else:
		sys.stderr.write("Set error!\n");
		sys.exit(56);
	return self.body;

def _expand_let(self):
	self.body = "";
	first  = self.args[self.args_ord_name[0]];
	second = self.args[self.args_ord_name[1]];
	if first[0] != '@' or second[0] != '@':
		exit_err("let macro requires macro names as both arguments", 57);
	if first == "@null":
		return self.body;
	if first == "@__def__" or first == "@__let__" or first == "@__set__":
		exit_err("Redef __macro__ error", 57);
	if second == "@null":
		if first in macro_list:
			del macro_list[first];
		return self.body;
	macro_list[first] = macro_list[second];
	return self.body;

def _expand_null(self):
	return "";