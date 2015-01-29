#!/usr/bin/python3

#JMP:xkozub03

import sys
import Config
import Reader
import Macro
from Config import exit_err

def usage():
	print("\nSkript pro realizaci jednoducheho makroprocesoru\n");
	print("Pouziti: jmp [--help] [--input=filename] [--output=filename]");
	print("             [--cmd=text] [-r]\n");
	print("--help               Vypis napovedy.");
	print("--input=filename     Vstupni soubor v kodovani ASCII.");
	print("--output=filename    Vystupni soubor v kodovani ASCII.");
	print("--cmd=text           Vlozi text na zacatek vstupu.");
	print("-r                   Redefinice pomoci @def zpusobi chybu.\n");

conf = Config.Config();

conf.parse_args(sys.argv);

if conf.get_help():
	usage();
	exit(0);

Macro.init_list(conf.get_redef_error());

text = conf.get_cmd_text();
text += conf.get_input().read();

reader = Reader.Reader(text);
reader.read_mode();

conf.get_output().write(reader.get_result());