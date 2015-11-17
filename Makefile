##########################################################################################
#
#   IFJ - interpret of IFJ15 language
#
#   Makefile
#   build binary file from source files
#
#   author:     Miroslav Karásek (xkaras31)
#   created:    2015-11-05
#   modified:   2015-11-05
#
##########################################################################################

# compiler options
CC=gcc
CFLAGS=-O2 -std=c99 -pedantic -Wall -Wextra -g


# all
all: ifj15


# modules compilation
main.o: main.c parser.h instlist.h interpret.h
	$(CC) $(CFLAGS) -c $< -o $@

instlist.o: instlist.c instlist.h
	$(CC) $(CFLAGS) -c $< -o $@

pointerstack.o: pointerstack.c pointerstack.h
	$(CC) $(CFLAGS) -c $< -o $@

parser.o: parser.c parser.h instlist.h
	$(CC) $(CFLAGS) -c $< -o $@


interpret.o: interpret.c interpret.h instlist.h pointerstack.h ial.h
	$(CC) $(CFLAGS) -c $< -o $@

ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -c $< -o $@


# link
ifj15: main.o instlist.o pointerstack.o parser.o interpret.o ial.o 
	$(CC) $(CFLAGS) $? -o $@


# clean
clean: 
	rm *.o *.exe ifj15