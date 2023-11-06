#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sched.h>
#include <fcntl.h>
#include <linux/sched.h>
#include <malloc.h>

#include "mythread.h"

int mythread_startup(void *arg) {
    mythread_struct_t *mythread = (mythread_t)arg;
    void *retval;

    //printf("mythread_startup: starting a thread func for the thread %d\n", mythread->id);
    retval = mythread->start_routine(mythread->arg);

    mythread->retval = retval;
    mythread->finished = 1;

    //printf("mythread_startup: waiting for join() the thread %d\n", mythread->id);
    while (!mythread->joined) {
        usleep(1);
    }
    //printf("mythread_startup: the thread func finished for the thread %d\n", mythread->id);

    if (munmap(mythread->stack, STACK_SIZE - PAGE) == -1) {
        perror("munmap");
    }

    return 0;
}

void *create_stack(off_t size) {
    void* stack;

    stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }
    
    return stack;
}

int mythread_create(mythread_t *mytid, void *(start_routine) (void *), void *arg) {
    mythread_struct_t *mythread;
    //int child_pid;
    void *child_stack;
    int flags;
    child_stack = create_stack(STACK_SIZE);
    if (child_stack == NULL) {
        fprintf(stderr, "create_stack() failed\n");
        return -1;
    }
    
    mythread = (mythread_struct_t*)(child_stack + STACK_SIZE - sizeof(mythread_struct_t));

    mythread->start_routine = start_routine;
    mythread->arg = arg;
    mythread->retval = NULL;
    mythread->joined = 0;
    mythread->finished = 0;
    mythread->stack = child_stack;
    
    //printf("mythread_create: creating thread %d\n", mythread->id);
    //printf("child stack %p; mythread_struct %p; \n", child_stack, mythread);

    child_stack = (void *)mythread;

    flags = CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD;
    mythread->id = clone(mythread_startup, child_stack, flags, (void*)mythread);
    if (mythread->id == -1) {
        perror("clone");
        return -1;
    }
    *mytid = mythread;

    return 0;
}

int mythread_join(mythread_t mytid, void **retval) {
    mythread_t mythread = mytid;

    while (!mythread->finished) {
        usleep(1);
    }
    
    //printf("mythread_join: the thread [%d] finished\n", mythread->id);

    *retval = mythread->retval;

    mythread->joined = 1;
    
    return 0;
}

int getmypid(mythread_t mytid) {
    mythread_t mythread = mytid;

    return mythread->id;
}
