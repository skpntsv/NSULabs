EXECUTABLES=main
SOURCES=./src/
TYPE=.c

CC=gcc
RM=rm
CFLAGS=-Wall
LIBS=src/mythreads/mythread.c
LDFLAGS=-o $@

TMPFILES=.stack.*

all: $(EXECUTABLES)

$(EXECUTABLES): %: $(SOURCES)%$(TYPE)
	$(CC) $(CFLAGS) ${LIBS} $(LDFLAGS) $<

run: $(EXECUTABLES)
	./${EXECUTABLES}

clean:
	${RM} -f *.o $(TMPFILES) $(EXECUTABLES)