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
DEPS = scanner.h instlist.h pointerstack.h interpret.h string.h ial.h parser.h
OBJ = parser.o interpret.o scanner.o instlist.o pointerstack.o string.o ial.o

# modules compilation
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# link
ifj15: $(OBJ)
	$(CC) main.c $(CFLAGS) $? -o $@
# test
test: test.c $(OBJ)
	$(CC) $(CFLAGS) $? -o $@

# clean
clean: 
	rm *.o *.exe ifj15