#
# Project: Implementace projektu do IMS 2014
# File: Makefile
# Created on: 22.11.2014
# Authors: Michal Kozubik (xkozub03)
# 	   Marek Hurta (xhurta01)
#

SRC_DIR = src
DOC_DIR = doc
BINARY = src/ims


.PHONY: all doc doxygen clean run pack

all:
	$(MAKE) -C $(SRC_DIR)
	$(MAKE) doxygen

run:
	$(MAKE)
	./$(BINARY)
doc:
	$(MAKE) -C $(DOC_DIR)

doxygen:
	$(MAKE) -C $(DOC_DIR)

pack:
	$(MAKE) clean
	zip -r xkozub03 $(SRC_DIR)/* $(DOC_DIR)/* Makefile

clean:
	$(MAKE) -i clean -C $(SRC_DIR)
	$(MAKE) clean -C $(DOC_DIR)

