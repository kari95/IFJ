##########################################################################################
#
#   IFJ - interpret of IFJ15 language
#
#   Makefile
#   build binary file from source files
#
#   author:     Miroslav Karásek (xkaras31)
#   created:    2015-11-05
#   modified:   2015-11-17
#
##########################################################################################

# compiler options
CC=gcc
CFLAGS=-O2 -std=c11 -pedantic -Wall -Wextra -g


# all
all: ifj15


# modules compilation
instlist.o: instlist.c instlist.h
	$(CC) $(CFLAGS) -c $< -o $@

pointerstack.o: pointerstack.c pointerstack.h
	$(CC) $(CFLAGS) -c $< -o $@

parser.o: parser.c parser.h instlist.h scanner.h string.h
	$(CC) $(CFLAGS) -c $< -o $@

scanner.o: scanner.c scanner.h string.h
	$(CC) $(CFLAGS) -c $< -o $@

interpret.o: interpret.c interpret.h instlist.h pointerstack.h ial.h
	$(CC) $(CFLAGS) -c $< -o $@

string.o: string.c string.h
	$(CC) $(CFLAGS) -c $< -o $@

ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -c $< -o $@


# link
ifj15: main.c scanner.o instlist.o pointerstack.o interpret.o string.o ial.o parser.o
	$(CC) $(CFLAGS) $? -o $@


# link test
test: test.c instlist.o pointerstack.o parser.o scanner.o interpret.o string.o ial.o 
	$(CC) $(CFLAGS) $? -o $@


# clean
clean: 
	rm *.o *.exe ifj15