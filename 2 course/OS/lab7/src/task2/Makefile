CC=gcc
CFLAGS=-Wall
LDFLAGS=-o $@
SOURCES=./source/
TYPE=.c
EXECUTABLES=server client

all: $(EXECUTABLES)

$(EXECUTABLES): %: $(SOURCES)%$(TYPE)
	$(CC) $(CFLAGS) $(LDFLAGS) $<

clean:
	rm -f $(EXECUTABLES)