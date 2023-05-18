CC=gcc
CFLAGS=-Wall
LDFLAGS=-o $@
SOURCES=./source/vars.c
EXECUTABLE=vars

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) 
	$(CC) $(LDFLAGS) $(SOURCES) $(CFLAGS)

clean:
	rm -f $(EXECUTABLE)
