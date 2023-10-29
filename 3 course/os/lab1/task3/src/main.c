#include <stdio.h>
#include <unistd.h>

#include "uthreads/uthread.h"

void thread1(void *arg) {
    char *str = (char *)arg;

    for (int i = 0; i < 5; i++) {
        printf("thread1[%d]: %s\n", i, str);
        sleep(1);

        uthread_sheduler();
    }
}

void thread2(void *arg) {
    char *str = (char *)arg;

    for (int i = 0; i < 5; i++) {
        printf("thread2[%d]: %s\n", i, str);
        sleep(1);

        uthread_sheduler();
    }
}

void thread3(void *arg) {
    char *str = (char *)arg;

    for (int i = 0; i < 5; i++) {
        printf("thread3[%d]: %s\n", i, str);
        sleep(1);

        uthread_sheduler();
    }
}

int main() {
    int err;
    uthread_t mytid1;
    uthread_t mytid2;
    uthread_t mytid3;

    uthread_t main_tid;

    uthread_manager.uthread_count = 0;  // Initialize the thread count
    uthread_manager.uthread_cur = 0;    // Initialize the current thread index

    init_thread(main_tid);

    printf("main [%d]\n", getpid());

    err = uthread_create(&mytid1, thread1, "hello from main1");
    if (err == -1) {
        fprintf(stderr, "uthread_create() failed\n");
    }
    err = uthread_create(&mytid2, thread2, "hello from main2");
    if (err == -1) {
        fprintf(stderr, "uthread_create() failed\n");
    }
    err = uthread_create(&mytid3, thread3, "hello from main3");
    if (err == -1) {
        fprintf(stderr, "uthread_create() failed\n");
    }

    while(1) {
        uthread_sheduler();
        int count = 0;
        for (int i = 0; i < 4; i++) {
            if (thread_is_finished(uthread_manager.uthreads[i])) {
                count++;
            }
            else {
                break;
            }
        }
        if (count == 4 - 1) {
            break;
        }
    }

    return 0;
}

