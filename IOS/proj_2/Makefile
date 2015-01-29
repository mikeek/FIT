# Makefile
# Reseni IOS-DU2, 30.3.2013
# Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
# Soubor: makefile

NAME1=santa

OBJFILES1=$(NAME1).o error.o

#CC=/usr/local/bin/x86_64-linux-x86_64-linux-gcc-4.5
CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

%.o : %.c
	$(CC) $(CFLAGS) -c $<

all: $(NAME1)

.PHONY: clean pack

error.o: error.c error.h

$(NAME1): $(OBJFILES1)
	$(CC) $(CFLAGS) $(OBJFILES1) -o $@

pack: 
	zip xkozub03.zip *.c *.h Makefile
clean:
	rm -f *.o