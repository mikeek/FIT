#
#  Project:  Implementace projektu do ICP
#  File: Makefile
#  Created on: 8. 3. 2014
#  Authors: Lukáš Huták (xhutak01)
#	    Michal Kozubík (xkozub03)
#

CLIENT=bludiste2014
CLIENT_CLI=bludiste2014-cli
SERVER=bludiste2014-server

SRC_DIR=./src
DOC_DIR=./doc
TST_DIR=./examples
MAP_DIR=./maps
DOXYFILE=doxyconf
DOXYCONF=$(DOC_DIR)/$(DOXYFILE)

.PHONY: all doxygen clean run pack

all:
	$(MAKE) -C $(SRC_DIR)
	$(MAKE) doxygen

run:
	$(MAKE)
	./$(CLIENT) &
	./$(SERVER) 4568

doxygen:
	$(MAKE) -C $(DOC_DIR)

pack:
	$(MAKE) clean
	zip -r xkozub03 $(SRC_DIR)/* $(DOC_DIR)/* $(TST_DIR)/* $(MAP_DIR)/* \
			README.txt Makefile

clean:
	$(MAKE) -i clean -C $(SRC_DIR)
	$(MAKE) clean -C $(DOC_DIR)
	rm -f $(CLIENT) $(SERVER) $(CLIENT_CLI)
