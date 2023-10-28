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

#define PAGE 4096
#define STACK_SIZE (PAGE * 5)

typedef void *(*start_routine_t)(void*);

typedef struct _mythread_t {
    int              id;
    start_routine_t  start_routine;
    void*            arg;
    void*            retval;

    volatile int     joined;
    volatile int     finished;
} mythread_struct_t;

typedef mythread_struct_t *mythread_t;

int mythread_startup(void *arg) {
    mythread_struct_t *mythread = (mythread_t)arg;
    void *retval;

    printf("mythread_startup: starting a thread func for the thread %d\n", mythread->id);
    retval = mythread->start_routine(mythread->arg);

    mythread->retval = retval;
    mythread->finished = 1;

    printf("mythread_startup: waiting for join() the thread %d\n", mythread->id);
    while(!mythread->joined) {
        usleep(1);
    }
    printf("mythread_startup: the thread func finished for the thread %d\n", mythread->id);

    return 0;
}

void *create_stack(off_t size, int mythread_id) {
    int stack_fd;
    void* stack;
    char stack_file[128];

    snprintf(stack_file , sizeof(stack_file), ".stack.%d", mythread_id);

    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    if (stack_fd == -1) {
        perror("open stack_fd");
        return NULL;
    }
    if (ftruncate(stack_fd, 0) == -1) {
        perror("ftruncate");
        return NULL;
    }
    if (ftruncate(stack_fd, size) == -1) {
        perror("ftruncate");
        return NULL;
    }

    stack = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, stack_fd, 0);
    if (stack == MAP_FAILED) {
       perror("mmap");
        return NULL;
    }

    if (close(stack_fd) == -1) {
        perror("close stack_fd");
        return NULL;
    }
    
    return stack;
}

int mythread_create(mythread_t *mytid, void *(start_routine) (void *), void *arg) {
    static int mythread_id = 0;
    mythread_t mythread;
    int child_pid;
    void *child_stack;
    int flags;

    mythread_id++; 

    printf("mythread_create: creating thread %d\n", mythread_id);

    child_stack = create_stack(STACK_SIZE, mythread_id);
    if (child_stack == NULL) {
        fprintf(stderr, "create_stack() failed\n");
        return -1;
    }
    if (mprotect(child_stack + PAGE, STACK_SIZE - PAGE, PROT_READ | PROT_WRITE) == -1) {
        perror("mprotect");
        return -1;
    }
    memset(child_stack + PAGE, 0x7f, STACK_SIZE - PAGE);

    mythread = (mythread_struct_t*)(child_stack + STACK_SIZE - sizeof(mythread_struct_t));
    mythread->id = mythread_id;
    mythread->start_routine = start_routine;
    mythread->arg = arg;
    mythread->retval = NULL;
    mythread->joined = 0;
    mythread->finished = 0;

    child_stack = (void *)mythread;

    printf("child stack %p; mythread_struct %p; \n", child_stack, mythread);

    flags = CLONE_VM | CLONE_FILES | 
            CLONE_THREAD | CLONE_SIGHAND;
    child_pid = clone(mythread_startup, child_stack, flags, (void*)mythread);
    if (child_pid == -1) {
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
    
    printf("mythread_join: the thread [%d] finished\n", mythread->id);

    *retval = mythread->retval;

    mythread->joined = 1;

    return 0;
}

void *mythread(void *arg) {
    char *str = (char *)arg;

	for (int i = 1; i < 5; i++) {
        printf("mythread: %s\n", str);
        sleep(1);
    }

    return "bay";
}

int main() {
    mythread_t mytid;
    int err;
    void *retval;

    printf("main [%d]\n", getpid());

    err = mythread_create(&mytid, mythread, "hello from main");
    if (err == -1) {
        fprintf(stderr, "mythread_create() failed\n");
    }

    mythread_join(mytid, &retval);

    printf("main [%d] thread returned %s\n", getpid(), (char*)retval);

	return 0;
}
