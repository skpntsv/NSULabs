EXECUTABLES=main
SOURCES=./
TYPE=.c

CC=gcc
RM=rm
CFLAGS=-Wall
LIBS=network_utils.c proxy.c http_message.c	
LDFLAGS=-o $@

all: $(EXECUTABLES)

$(EXECUTABLES): %: $(SOURCES)%$(TYPE)
	$(CC) $(CFLAGS) $(LDFLAGS) $< ${LIBS}

run: $(EXECUTABLES)
	./${EXECUTABLES}

clean:
	${RM} -f *.o $(EXECUTABLES)