#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


void *thread_1(void* arg) {
    sigset_t set;
    if (sigfillset(&set) == -1) {
        perror("thread_1: sigfillset() failed");
        return NULL;
    }
    if (pthread_sigmask(SIG_BLOCK, &set, NULL)) {
        perror("thread_1: pthread_sigmask() failed");
        return NULL;
    }

    printf("thread_1: thread blocked signals\n");

    sleep(100);
    printf("thread_1: thread has finished\n");
    return NULL;
}

void sigint_handler(int sig) {
    printf("Signal received: %d\n", sig);
}

void *thread_2(void * arg) {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("thread_2: signal() failed");
        return NULL;
    }

    return NULL;
}

void *thread_3(void * arg) {
    int sig;

    sigset_t set;
    if (sigemptyset(&set) == -1) {
        perror("thread_3: sigemptyset() failed");
        return NULL;
    }
    if (sigaddset(&set, SIGQUIT) == -1) {
        perror("thread_3: sigaddset failed");
        return NULL;
    }

    printf("thread_3: thread is waiting for a SIGQUIT signal\n");

    while (1) {
        if (sigwait(&set, &sig) != 0) {
            perror("thread_3: sigwait() failed");
            return NULL;
        }
        printf("thread_3: thread received SIGQUIT signal: %d\n", sig);
    }
    return NULL;
}

int main() {
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    printf("main: pid:%d\n", getpid());

    pthread_create(&tid1, NULL, thread_1, NULL);
    pthread_create(&tid2, NULL, thread_2, NULL);
    pthread_create(&tid3, NULL, thread_3, NULL);

    pthread_exit(0);
    return 0;
}
