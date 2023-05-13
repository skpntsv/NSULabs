CC=gcc
CFLAGS=-Wall
LDFLAGS=-o $@
SOURCES=./source/
TYPE=.c
EXECUTABLES=exec manage

all: $(EXECUTABLES)

$(EXECUTABLES): %: $(SOURCES)%$(TYPE)
	$(CC) $(CFLAGS) $(LDFLAGS) $<

clean:
	rm -f $(EXECUTABLES)
