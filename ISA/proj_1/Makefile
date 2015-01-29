# author - Michal Kozubik, xkozub03@stud.fit.vutbr.cz

CC=g++
CPPFLAGS=-Wextra -pedantic -Wall -O2 --std=c++11 -Wno-missing-field-initializers -g

SOURCES=$(wildcard *.cpp)
OBJFILES=$(SOURCES:.cpp=.o)
HEADERS=Client.h Communicator.h
BINARY=mytftpclient

all: $(BINARY)

$(BINARY): $(OBJFILES) $(HEADERS)
	$(LINK.o) $^ -o $@ $(CPPFLAGS)

clean:
	rm -f $(BINARY) $(OBJFILES)
