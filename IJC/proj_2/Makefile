# Makefile
# Reseni IJC-DU1, 28.2.2013
# Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
# Soubor: makefile

NAME1=tail
NAME2=tail2
NAME3S=wordcount
NAME3D=wordcount-dynamic
LIBS=libhtable-s
LIBD=libhtable-d

OBJFILES1=$(NAME1).o error.o
OBJFILES2=$(NAME2).o
OBJFILES3=$(NAME3S).o error.o io.o
LIBSOBJ=hash_function.o htable_clear.o htable_foreach.o htable_free.o htable_init.o htable_lookup.o htable_remove.o htable_statistics.o

#CC=/usr/local/bin/x86_64-linux-x86_64-linux-gcc-4.5
CC=gcc
#CPP=g++-4.7
CPP=g++
CFLAGS=-std=c99 -Wall -pedantic -g 
CPPFLAGS=-std=c++11 -Wall -pedantic

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.cc
	$(CPP) $(CPPFLAGS) -c $<

all: $(LIBS) $(LIBD) $(NAME1) $(NAME2) $(NAME3S) $(NAME3D)

.PHONY: clean pack

error.o: error.c error.h
io.o: io.c io.h
$(NAME1).o: $(NAME1).c error.h
$(NAME2).o: $(NAME2).cc
$(NAME3S).o: $(NAME3S).c htable.h error.h io.h
hash_function.o: hash_function.c htable.h
	$(CC) $(CFLAGS) -c -fPIC $<
htable_clear.o: htable_clear.c htable.h
	$(CC) $(CFLAGS) -c -fPIC $<
htable_foreach.o: htable_foreach.c htable.h
	$(CC) $(CFLAGS) -c -fPIC $<
htable_free.o: htable_free.c htable.h
	$(CC) $(CFLAGS) -c -fPIC $<
htable_init.o: htable_init.c htable.h
	$(CC) $(CFLAGS) -c -fPIC $<
htable_lookup.o: htable_lookup.c htable.h
	$(CC) $(CFLAGS) -c -fPIC $<
htable_remove.o: htable_remove.c htable.h
	$(CC) $(CFLAGS) -c -fPIC $<
htable_statistics.o: htable_statistics.c htable.h
	$(CC) $(CFLAGS) -c -fPIC $<

$(LIBS): $(LIBSOBJ)
	ar rsc libhtable.a $(LIBSOBJ)

$(LIBD): $(LIBSOBJ)
	$(CC) -shared -fPIC -o libhtable.so $(LIBSOBJ)

$(NAME1): $(OBJFILES1)
	$(CC) $(CFLAGS) $(OBJFILES1) -o $@

$(NAME2): $(OBJFILES2)
	$(CPP) $(CPPFLAGS) $(OBJFILES2) -o $@

$(NAME3S): $(OBJFILES3)
	$(CC) $(CFLAGS) -static $(OBJFILES3) -L. -lhtable -o $@

$(NAME3D): $(OBJFILES3)
	$(CC) $(CFLAGS) $(OBJFILES3) -L. -lhtable -o $@

pack:
	zip xkozub03.zip *.c *.cc *.h Makefile

clean:
	rm -f *.o