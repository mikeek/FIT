#
#  Project:  Implementace interpretu imperativního jazyka IFJ13.
#  File: Makefile
#  Created on: 1. 10. 2013
#  Authors: Michal Kozubík, xkozub03
#

#CC=/usr/local/bin/x86_64-linux-x86_64-linux-gcc-4.5
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -O3
SOURCES=common.c ial.c interpret.c main.c parser.c scanner.c \
	tree.c str.c generator.c builtin.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test

all: $(SOURCES) $(EXECUTABLE)

.PHONY: clean pack

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

common.o: common.c common.h scanner.h
ial.o: ial.c ial.h
interpret.o: interpret.c interpret.h common.h
parser.o: parser.c parser.h scanner.h common.h tree.h generator.h ial.h
scanner.o: scanner.c scanner.h common.h
str.o: str.c str.h
tree.o: tree.c tree.h
generator.o: generator.c generator.h ial.h str.h
builtin.o: builtin.c builtin.h common.h

pack:
	zip xhutak01.zip *.c *.h Makefile rozdeleni rozsireni

clean:
	rm -f *.o
