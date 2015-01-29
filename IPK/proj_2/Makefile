# Makefile
# Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
# IPK projekt c. 2

NAME_C=client
NAME_S=server

OBJFILES_C=$(NAME_C).o Client_class.o Parser.o
OBJFILES_S=$(NAME_S).o Server_class.o Parser.o

CPP=g++48
#CPP=g++
CPPFLAGS=-std=c++11 -Wall -Wextra -Werror -pedantic -static-libstdc++

all: $(NAME_C) $(NAME_S)

.PHONY: pack

Client_class.o: Client_class.cpp
	$(CPP) $(CPPFLAGS) -c -o Client_class.o Client_class.cpp

Server_class.o: Server_class.cpp
	$(CPP) $(CPPFLAGS) -c -o Server_class.o Server_class.cpp

Parser.o: Parser.cpp
	$(CPP) $(CPPFLAGS) -c -o Parser.o Parser.cpp


$(NAME_C).o: $(NAME_S).cpp
	$(CPP) $(CPPFLAGS) -c -o $(NAME_C).o $(NAME_C).cpp

$(NAME_S).o: $(NAME_S).cpp
	$(CPP) $(CPPFLAGS) -c -o $(NAME_S).o $(NAME_S).cpp

$(NAME_C): $(OBJFILES_C)
	$(CPP) $(CPPFLAGS) $(OBJFILES_C) -o $(NAME_C)

$(NAME_S): $(OBJFILES_S)
	$(CPP) $(CPPFLAGS) $(OBJFILES_S) -pthread -o $(NAME_S)

pack:
	tar -cvzf xkozub03.tar.gz *.cpp *.h Makefile
clean:
	rm -f *.o
