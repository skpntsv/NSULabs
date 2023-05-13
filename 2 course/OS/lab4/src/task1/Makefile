CC=gcc
CFLAGS=-Wall
LDFLAGS=-o $@
SOURCES=./source/vars.c
EXECUTABLE=vars

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) 
	$(CC) $(LDFLAGS) $(SOURCES) $(CFLAGS)
	export PORT=1000

clean:
	rm -f $(EXECUTABLE)
