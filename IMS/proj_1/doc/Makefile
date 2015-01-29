#
#  Project: Implementace projektu do IMS 2014
#  File: Makefile
#  Created on: 22.11.2014
#  Authors: Michal Kozubík (xkozub03)
# 	    Marek Hurta (xhurta01)
#

DOXYFILE = doxyconf

.PHONY: all clean

all:
	doxygen $(DOXYFILE)

clean:
	rm -rf html/*
	mkdir -p html && rmdir html

