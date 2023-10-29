EXECUTABLES=main
SOURCES=./src/
TYPE=.c

CC=gcc
RM=rm
CFLAGS=-Wall
LIBS=src/uthreads/uthread.c
LDFLAGS=-o $@

TMPFILES=.stack.*

all: $(EXECUTABLES)

$(EXECUTABLES): %: $(SOURCES)%$(TYPE)
	$(CC) $(CFLAGS) $(LDFLAGS) $< ${LIBS}

run: $(EXECUTABLES)
	./${EXECUTABLES}

clean:
	${RM} -f *.o $(TMPFILES) $(EXECUTABLES)