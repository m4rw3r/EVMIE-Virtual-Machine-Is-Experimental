
SOURCE := $(wildcard src/*.c)
OBJS   := $(patsubst %.c,%.o,$(SOURCE))
CC     := gcc
CFLAGS := -std=c99 -Wall -Wextra --pedantic -O3 -I. -g
LFLAGS :=

main: $(OBJS)
	$(CC) $(LFLAGS) -o main $(OBJS)

all: main

clean:
	rm -f src/*.o
	rm -f main

distclean: clean
	rm -f config.h
	rm -f config.log
	rm -f config.status
	rm -rf autom4te.cache

dist: distclean
	rm -f configure
	rm -f config.h.in
	autoheader
	autoconf
