#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void my_free(void* arg){
	free(arg);
	printf("successful cleaning memory\n");
}

void *thread_malloc(void* arg){
	char* hello = (char*)malloc(sizeof(char) * 13);
	strcpy(hello, "Hello world\n");

	pthread_cleanup_push(my_free, hello);
	while(1) {
		printf("%s", hello);
	}

	pthread_cleanup_pop(1);
}

int main() {
	pthread_t tid;
	int err;

	printf("main [%d]: Hello from main!\n", getpid());

	err = pthread_create(&tid, NULL, thread_malloc, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

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
