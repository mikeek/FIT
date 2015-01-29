# Makefile
# Reseni IJC-DU1, 28.2.2013
# Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
# Soubor: makefile

NAME1=prvocisla
NAME2=prvocisla-inline
NAME3=steg-decode

OBJFILES1=$(NAME1).o eratosthenes.o error.o
OBJFILES3=$(NAME3).o eratosthenes.o error.o ppm.o

#CC=/usr/local/bin/x86_64-linux-x86_64-linux-gcc-4.5
CC=gcc
CFLAGS=-O2 -std=c99 -Wall -pedantic

%.o : %.c
	$(CC) $(CFLAGS) -c $<

all: $(NAME1) $(NAME2) $(NAME3)

eratosthenes.o: eratosthenes.c eratosthenes.h
error.o: error.c error.h
ppm.o: ppm.c ppm.h
prvocisla.o: prvocisla.c bit-array.h eratosthenes.h error.h
steg-decode.o: steg-decode.c bit-array.h eratosthenes.h error.h ppm.h

$(NAME1): $(OBJFILES1)
	$(CC) $(CFLAGS) $(OBJFILES1) -o $@ -lm

$(NAME2): $(OBJFILES1)
	$(CC) $(CFLAGS) $(OBJFILES1) -DUSE_INLINE -o prvocisla-inline -lm

$(NAME3): $(OBJFILES3)
	$(CC) $(CFLAGS) $(OBJFILES3) -o $@ -lm

clean:
	rm -f *.o