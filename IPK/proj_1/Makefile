# Makefile
# Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
# IPK projekt c. 1

NAME=ftpclient

OBJFILES=$(NAME).o

#CPP=g++48
CPP=g++
CPPFLAGS=-std=c++11 -Wall -Wextra -Werror -pedantic #-static-libstdc++

all: $(NAME)

.PHONY: pack

$(NAME).o : $(NAME).cpp
	$(CPP) $(CPPFLAGS) -c -o $(NAME).o $(NAME).cpp

$(NAME): $(OBJFILES)
	$(CPP) $(CPPFLAGS) $(OBJFILES) -o $(NAME)

pack:
	tar -cvzf xkozub03.tar.gz $(NAME).cpp Makefile
clean:
	rm -f *.o
