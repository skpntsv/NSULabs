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
    struct ThreadData data;

    data.number = 42;
    data.message = "Hello from struct!";

	err = pthread_create(&tid, NULL, mythread, &data);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    if (pthread_join(tid, NULL) != 0) {
        printf("Main: pthread_join() failed\n");
        return -1;
    }

	return 0;
}
