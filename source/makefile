
EXEC ?= choster
LIB ?= libcohost.a
RM ?= rm -f
CC ?= gcc
AR ?= ar
PKGCONFIG ?= pkg-config

# base flags
override CFLAGS += -std=c89 -pedantic -Wextra -Wall -Wno-sign-compare

# curl flags
override CFLAGS += $(shell $(PKGCONFIG) libcurl --cflags)
override LDFLAGS += $(shell $(PKGCONFIG) libcurl --libs)

ifeq ($(DEBUG),1)
override CFLAGS += -g3
endif

EXEC_OBJECTS = main.o
LIB_OBJECTS = libcohost.o

all: clean $(EXEC) $(LIB)

clean:
	$(RM) $(EXEC_OBJECTS) $(EXEC) $(LIB)

$(EXEC): $(LIB) $(EXEC_OBJECTS)
	$(CC) -o $@ $^ $(LIB) $(LDFLAGS)

$(LIB): $(LIB_OBJECTS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
