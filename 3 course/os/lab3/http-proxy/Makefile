EXECUTABLES=proxy
# SOURCES=./src/
SOURCES=./
TYPE=.c

CC=gcc
RM=rm
CFLAGS=-Wall
# LIBS=src/uthreads/uthread.c
LIBS=
LDFLAGS=-o $@

all: $(EXECUTABLES)

$(EXECUTABLES): %: $(SOURCES)%$(TYPE)
	$(CC) $(CFLAGS) $(LDFLAGS) $< ${LIBS}

run: $(EXECUTABLES)
	./${EXECUTABLES}

clean:
	${RM} -f *.o $(EXECUTABLES)