EXECUTABLES=mutex
SOURCES=./src/

TYPE=.c
CC=gcc
RM=rm
CFLAGS=-Wall
LIBS=linkedlist/linkedlist.c
LDFLAGS=-o $@

all: $(EXECUTABLES)

$(EXECUTABLES): %: $(SOURCES)%$(TYPE)
	$(CC) $(CFLAGS) $(LDFLAGS) $< ${LIBS}

run: $(EXECUTABLES)
	./${EXECUTABLES}

clean:
	${RM} -f *.o $(EXECUTABLES)