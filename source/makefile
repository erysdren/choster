
TARGET ?= choster
RM ?= rm -f
CC ?= gcc
PKGCONFIG ?= pkg-config

override CFLAGS += $(shell $(PKGCONFIG) pdcurses --cflags)
override LDFLAGS += $(shell $(PKGCONFIG) pdcurses --libs)

OBJS = main.o

all: clean $(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
