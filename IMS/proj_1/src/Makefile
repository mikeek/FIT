#
# Project: Implementace projektu do IMS 2014
# File: Makefile
# Created on: 22.11.2014
# Authors: Michal Kozubik (xkozub03)
# 	   Marek Hurta (xhurta01)
#

CC=g++
CPPFLAGS=-Wextra -pedantic -Wall -O2 --std=c++11 -Wno-missing-field-initializers -g
LDFLAGS=-lsimlib -lm

SOURCES=$(wildcard *.cpp)
OBJFILES=$(SOURCES:.cpp=.o)
HEADERS=$(wildcard *.h)
BINARY=ims

all: $(BINARY)

$(BINARY): $(OBJFILES) $(HEADERS)
	$(LINK.o) $^ -o $@ $(CPPFLAGS) $(LDFLAGS)

clean:
	rm -f $(BINARY) $(OBJFILES)
