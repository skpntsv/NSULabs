EXECUTABLES=main
SOURCES=./
TYPE=.c

CC=gcc
RM=rm
CFLAGS=-Wall
FILES=networks/network_utils.c networks/http_message.c proxy.c
LIBS=-lpthread
LDFLAGS=-o $@

all: $(EXECUTABLES)

$(EXECUTABLES): %: $(SOURCES)%$(TYPE)
	$(CC) $(CFLAGS) $(LDFLAGS) $< ${LIBS} ${FILES}

run: $(EXECUTABLES)
	./${EXECUTABLES}

clean:
	${RM} -f *.o $(EXECUTABLES)