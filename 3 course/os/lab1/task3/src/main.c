#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "uthreads/uthread.h"

#define THREADS 4

void thread1(void *arg, uthread_manager_t *uthread_manager) {
    char *str = (char *)arg;

    for (int i = 0; i < 5; i++) {
        printf("thread1[%d]: %s\n", i, str);
        sleep(1);

        uthread_sheduler(uthread_manager);
    }
}

void thread2(void *arg, uthread_manager_t *uthread_manager) {
    char *str = (char *)arg;

    for (int i = 0; i < 5; i++) {
        printf("thread2[%d]: %s\n", i, str);
        sleep(1);

        uthread_sheduler(uthread_manager);
    }
}

void thread3(void *arg, uthread_manager_t *uthread_manager) {
    char *str = (char *)arg;

    for (int i = 0; i < 5; i++) {
        printf("thread3[%d]: %s\n", i, str);
        sleep(1);

        uthread_sheduler(uthread_manager);
    }
}

int main() {
    int err;

    uthread_t mythreads[THREADS];
    uthread_t main_thread;
    uthread_manager_t *uthread_manager = (uthread_manager_t *)malloc(sizeof(uthread_manager_t));

    init_thread(&main_thread, uthread_manager);
    mythreads[0] = main_thread;

    printf("main [%d]\n", getpid());

    err = uthread_create(&mythreads[1], uthread_manager, thread1, "hello from main1");
    if (err == -1) {
        fprintf(stderr, "uthread_create() failed\n");
    }
    err = uthread_create(&mythreads[2], uthread_manager, thread2, "hello from main2");
    if (err == -1) {
        fprintf(stderr, "uthread_create() failed\n");
    }
    err = uthread_create(&mythreads[3], uthread_manager, thread3, "hello from main3");
    if (err == -1) {
        fprintf(stderr, "uthread_create() failed\n");
    }

    while (1) {
        int count = 0;
        for (int i = 1; i < THREADS; i++) {
            if (thread_is_finished(mythreads[i])) {
                count++;
            }
        }
        if (count == THREADS - 1) {
            break;
        }
        uthread_sheduler(uthread_manager);
    }

    free(uthread_manager);

    return 0;
}

