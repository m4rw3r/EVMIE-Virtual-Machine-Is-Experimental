
SOURCE  := $(wildcard src/*.c)
OBJS    := $(patsubst %.c,%.o,$(SOURCE))
CFLAGS  := -std=c99 -Wall -Wextra --pedantic -O3 -I. -g
LFLAGS  :=

# Adds the LeakCheck tool to watch for leaked memory
# NOTE: Needs a call to LeakCheck_printMemReport(boolean free_leak_info)
#       to print LeakCheck information
ifdef LEAKCHECK
	CFLAGS += -include tools/LeakCheck.h
	OBJS += tools/LeakCheck.o
endif

tools/LeakCheck.o:
	$(CC) -std=c99 -Wall -Wextra --pedantic -O3 -g -c -o tools/LeakCheck.o tools/LeakCheck.c

main: $(OBJS)
	$(CC) $(LFLAGS) -o main $(OBJS)

all: main

clean:
	rm -f tools/*.o
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