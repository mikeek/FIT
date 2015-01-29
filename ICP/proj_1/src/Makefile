#
#  Project:  Implementace projektu do ICP
#  File: Makefile
#  Created on: 8. 3. 2014
#  Authors: Lukáš Huták (xhutak01)
#	    Michal Kozubík (xkozub03)
#

HOSTNAME=$(shell hostname)

ifeq ($(HOSTNAME),merlin.fit.vutbr.cz)
	QMAKE=/usr/local/share/Qt/5.2.1/gcc_64/bin/qmake
	CXX=g++-4.9
else
	QMAKE=qmake
	CXX=g++
endif

CLIENT=bludiste2014
SERVER=bludiste2014-server
CLIENT_CLI=bludiste2014-cli

CXXFLAGS=-Wall -Wextra -pedantic -std=c++11 -lpthread -lboost_system 

CLI_SOURCES=bludiste2014-cli.cpp Client.cpp
SERVER_SOURCES=bludiste2014-server.cpp Server.cpp Map.cpp Game.cpp

CLI_OBJS=$(CLI_SOURCES:.cpp=.o)
SERVER_OBJS=$(SERVER_SOURCES:.cpp=.o)

#$^ - vsechny zavislosti
#$@ - vysledny nazev
#LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)


all: $(CLIENT) $(CLIENT_CLI) $(SERVER)

$(CLIENT):
	@echo "QMAKE_CXX=$(CXX)">icp.pri
	$(QMAKE) -makefile icp.pro
	make -f QMakefile
	mv $(CLIENT) ../

$(CLIENT_CLI): $(CLI_OBJS)
	$(LINK.cc) -lmenu -lncurses $^ -o ../$@

$(SERVER): $(SERVER_OBJS)
	$(LINK.cc) $^ -o ../$@


clean:
	make -f QMakefile clean
	rm -f *.o QMakefile
