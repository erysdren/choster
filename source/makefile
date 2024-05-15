
EXEC ?= choster
LIB ?= libcohost.a
RM ?= rm -f
CC ?= gcc
AR ?= ar
PKGCONFIG ?= pkg-config

# base flags
override CFLAGS += -pedantic -Wextra -Wall

# curl flags
override CFLAGS += $(shell $(PKGCONFIG) libcurl sdl2 --cflags)
override LDFLAGS += $(shell $(PKGCONFIG) libcurl sdl2 --libs)

ifeq ($(DEBUG),1)
override CFLAGS += -g3
endif

ifeq ($(RELEASE),1)
override CFLAGS += -O3
endif

EUI_OBJECTS = eui/eui.o eui/eui_evnt.o eui/eui_sdl2.o eui/eui_widg.o
EXEC_OBJECTS = main.o $(EUI_OBJECTS)
LIB_OBJECTS = libcohost.o thirdparty/cJSON.o

all: clean $(EXEC) $(LIB)

clean:
	$(RM) $(EXEC_OBJECTS) $(EXEC) $(LIB)

$(EXEC): $(LIB) $(EXEC_OBJECTS)
	$(CC) -o $@ $^ $(LIB) $(LDFLAGS)

$(LIB): $(LIB_OBJECTS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
