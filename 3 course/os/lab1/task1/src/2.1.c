#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void *thread_with_number(void *arg) {
    static int number = 42;
    pthread_exit(&number);
}

void *thread_with_message(void *arg) {
    char *message = "hello world";
    pthread_exit(message);
}

int main() {
    pthread_t tid_number, tid_message;
    int err;
    int *number_result;
    char *message_result;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), getpid());

    err = pthread_create(&tid_number, NULL, thread_with_number, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid_message, NULL, thread_with_message, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_join(tid_number, (void **)&number_result);
    pthread_join(tid_message, (void **)&message_result);

    printf("main: Child thread returned %d\n", *number_result);
    printf("Main thread: Child thread returned: %s\n", message_result);

    return 0;
}
