## ========================================================
##
## FILE:			/Makefile
##
## AUTHORS:			Jaycie Ewald
##
## PROJECT:			libcohost
##
## LICENSE:			ACSL v1.4
##
## DESCRIPTION:		libcohost make file
##
## LAST EDITED:		November 20th, 2022
##
## ========================================================

## Variables
CC = gcc

COHOST_DEPS = cohost.h
COHOST_EXEC = cohost
COHOST_OBJS = base64.o cohost.o hash.o main.o network.o response.o salt.o utils.o
COHOST_LIBS = -lcurl -l:libcjson.a -l:libnettle.a
COHOST_FLGS =

## Executable
$(COHOST_EXEC): $(COHOST_OBJS)
	$(CC) $(COHOST_OBJS) -o $(COHOST_EXEC) $(COHOST_LIBS)

## Rule for C objects
%.o: %.c $(COHOST_DEPS)
	$(CC) -c -o $@ $< $(COHOST_FLGS)

## Clean up
clean:
	rm $(COHOST_EXEC) $(COHOST_OBJS)
