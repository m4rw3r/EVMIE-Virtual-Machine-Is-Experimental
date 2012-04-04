
SOURCE  := $(wildcard src/*.c)
OBJS    := $(patsubst %.c,%.o,$(SOURCE))
CFLAGS  := -std=c99 -Wall -Wextra --pedantic -O3 -I. -g
LFLAGS  :=

OPCOMPILER_SOURCE := tools/OpCompiler.c \
	tools/Token.c \
	tools/Opcode.c
OPCOMPILER_OBJS   := $(patsubst %.c,%.o,$(OPCOMPILER_SOURCE))

# Adds the LeakCheck tool to watch for leaked memory
# NOTE: Needs a call to LeakCheck_printMemReport(boolean free_leak_info)
#       to print LeakCheck information unless exit(status) is used on exit
ifdef LEAKCHECK
	CFLAGS += -include tools/LeakCheck.h
	OBJS += tools/LeakCheck.o
	OPCOMPILER_OBJS += tools/LeakCheck.o
endif

all: main

tools/LeakCheck.o:
	$(CC) -std=c99 -Wall -Wextra --pedantic -O3 -g -c -o tools/LeakCheck.o tools/LeakCheck.c

main: $(OBJS)
	$(CC) $(LFLAGS) -o main $(OBJS)
opcompiler: $(OPCOMPILER_OBJS)
	$(CC) $(LFLAGS) $(OPCOMPILER_OBJS) -o tools/OpCompiler


clean:
	rm -f tools/*.o
	rm -f tools/OpCompiler
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