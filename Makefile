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


# link
ifj15: main.o
	$(CC) $(CFLAGS) $? -o $@


# clean
clean: 
	rm *.o *.exe ifj15