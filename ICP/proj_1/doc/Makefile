#
#  Project:  Implementace projektu do ICP
#  File: Makefile
#  Created on: 8. 3. 2014
#  Authors: Michal Kozubík (xkozub03)
#

DOXYFILE=doxyconf

.PHONY: all clean

all:
	doxygen $(DOXYFILE)

clean:
	rm -rf html/*
	mkdir -p html && rmdir html
