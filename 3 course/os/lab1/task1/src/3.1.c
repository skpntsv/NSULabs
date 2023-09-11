#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

struct ThreadData {
    int number;
    char* message;
};

void *mythread(void *arg) {
	struct ThreadData *data = (struct ThreadData*)arg;
    printf("mythread: Number = %d, Message = %s\n", data->number, data->message);
    return NULL;
}

int main() {
	pthread_t tid;
	int err;
    
    struct ThreadData *data = (struct ThreadData *)malloc(sizeof(struct ThreadData));
    if (data == NULL) {
        perror("malloc");
    }

    data->number = 42;
    data->message = "Hello from main!";
	printf("main [%d]: Hello from main!\n", getpid());

	err = pthread_create(&tid, NULL, mythread, data);
    pthread_join(tid, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
    
    free(data);
	return 0;
}
