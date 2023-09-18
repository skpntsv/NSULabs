#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    printf("Thread [%lu]: Hello from mythread!\n", (unsigned long)pthread_self());
    //pthread_detach(pthread_self());  // Отсоединяем поток
    return NULL;
}

int main() {
    int k = 0;
    while (1) {
        pthread_t tid;
        int err;

        err = pthread_create(&tid, NULL, mythread, NULL);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
		    return -1;
        }
        k++;
        // 19665
        printf("k = %d\n", k);
    }

    return 0;
}
