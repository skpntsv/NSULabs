#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *thread_loop(void *arg) {
	while(1) {
        printf("%s\n", "Hello world!");
        usleep(500);
    }

    return NULL;
}

int main() {
	pthread_t tid;
	int err;

	printf("main [%d]: Hello from main!\n", getpid());

	err = pthread_create(&tid, NULL, thread_loop, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
    sleep(2);   // даем поработать потоку несколько секунд

    printf("main: cancelling thread\n");
    err = pthread_cancel(tid);
    if (err) {
        printf("main: pthread_cancel() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(tid, NULL);
    if (err) {
        printf("Main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

	return 0;
}
