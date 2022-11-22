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
## LAST EDITED:		November 22nd, 2022
##
## ========================================================

## Variables
CC = gcc

COHOST_DEPS = cohost.h
COHOST_EXEC = cohost
COHOST_OBJS = cohost.o main.o
COHOST_LIBS = -lcurl -l:libcjson.a -l:libnettle.a
COHOST_FLGS = -g

## Executable
$(COHOST_EXEC): $(COHOST_OBJS)
	$(CC) $(COHOST_FLGS) $(COHOST_OBJS) -o $(COHOST_EXEC) $(COHOST_LIBS)

## Rule for C objects
%.o: %.c $(COHOST_DEPS)
	$(CC) -c -o $@ $< $(COHOST_FLGS)

## Clean up
clean:
	rm $(COHOST_EXEC) $(COHOST_OBJS)
