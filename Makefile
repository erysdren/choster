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
## DESCRIPTION:		libcohost makefile
##
## LAST EDITED:		November 23rd, 2022
##
## ========================================================

ifdef DJGPP

CC = i586-pc-msdosdjgpp-gcc

COHOST_EXEC = ./build/dos/cohost.exe

COHOST_LIBS = -l:libcurl.a -l:libwatt.a -l:libcjson.a
COHOST_FLGS = -g -L./libs -I./thirdparty/curl/include -I./thirdparty/watt/inc -I./thirdparty

else

CC = gcc

COHOST_EXEC = ./build/cohost

COHOST_LIBS = -lcurl -l:libcjson.a -l:libnettle.a
COHOST_FLGS = -g

endif

## Variables
COHOST_DEPS = cohost.h
COHOST_OBJS = cohost.o main.o

## Executable
$(COHOST_EXEC): $(COHOST_OBJS)
	$(CC) $(COHOST_FLGS) $(COHOST_OBJS) -o $(COHOST_EXEC) $(COHOST_LIBS)

## Rule for C objects
%.o: %.c $(COHOST_DEPS)
	$(CC) -c -o $@ $< $(COHOST_FLGS)

## Clean up
clean:
	rm $(COHOST_EXEC) $(COHOST_OBJS)
