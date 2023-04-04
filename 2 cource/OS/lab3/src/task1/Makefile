all: replica

replica: replicator
	./replicator ridtest
replicator: ./source/replicate.c
	gcc -o replicator ./source/replicate.c -Wall

clean:
	rm -rf replicator tsetdir
