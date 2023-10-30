#include <stdio.h>
#include <unistd.h>

#include "mythreads/mythread.h"

void *mythread1(void *arg) {
    char *str = (char *)arg;

	for (int i = 0; i < 1; i++) {
        printf("Mythread1[%d]: %s\n", i, str);
        sleep(1);
    }

    return "bay from 1";
}

void *mythread2(void *arg) {
    char *str = (char *)arg;

	for (int i = 0; i < 3; i++) {
        printf("Mythread2[%d]: %s\n", i, str);
        sleep(1);
    }

    return "bay from 2";
}

void *mythread3(void *arg) {
    char *str = (char *)arg;

	for (int i = 0; i < 4; i++) {
        printf("Mythread3[%d]: %s\n", i, str);
        sleep(1);
    }

    return "bay from 3";
}

int main() {
    mythread_t mytid1;
    mythread_t mytid2;
    mythread_t mytid3;
    int err;
    void *retval1;
    void *retval2;
    void *retval3;

    printf("main [%d]\n", getpid());

    err = mythread_create(&mytid1, mythread1, "hello from main1");
    if (err == -1) {
        fprintf(stderr, "mythread_create() failed\n");
    }
    err = mythread_create(&mytid2, mythread2, "hello from main2");
    if (err == -1) {
        fprintf(stderr, "mythread_create() failed\n");
    }
    err = mythread_create(&mytid3, mythread3, "hello from main3");
    if (err == -1) {
        fprintf(stderr, "mythread_create() failed\n");
    }

    mythread_join(mytid1, &retval1);
    mythread_join(mytid2, &retval2);
    mythread_join(mytid3, &retval3);

    printf("main [%d] thread[%d] returned %s\n", getpid(), getmypid(mytid1), (char*)retval1);
    printf("main [%d] thread[%d] returned %s\n", getpid(), getmypid(mytid2), (char*)retval2);
    printf("main [%d] thread[%d] returned %s\n", getpid(), getmypid(mytid3), (char*)retval3);

	return 0;
}