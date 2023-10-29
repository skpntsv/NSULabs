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
#include <ucontext.h>

#define PAGE 4096
#define STACK_SIZE (PAGE * 5)
#define MAX_THREADS 8

typedef struct _uthread_manager_t {
    uthread_struct_t *uthreads[MAX_THREADS];
    int uthread_count;
    int uthread_cur;
} uthread_manager_t;

uthread_manager_t uthread_manager;

typedef struct _uthread_struct_t {
    int              uthread_id;
    void             (*thread_func)(void*);
    void             *arg;
    void             *retval;

    volatile int     finished;
    ucontext_t       uctx;
} uthread_struct_t;

typedef uthread_struct_t *uthread_t;

int uthread_startup(void *arg) {
    uthread_struct_t *uthread = (uthread_t)arg;
    void *retval;

    printf("uthread_startup: starting a thread func for the thread %d\n", uthread->uthread_id);
    uthread->thread_func(uthread->arg);
    
    uthread->retval = retval;
    uthread->finished = 1;

    return 0;
}

void *create_stack(off_t size, int uthread_id) {
    int stack_fd;
    void* stack;
    char stack_file[128];

    snprintf(stack_file , sizeof(stack_file), ".stack.%d", uthread_id);

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

    stack = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_STACK, stack_fd, 0);
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

void uthread_sheduler() {
    int err;
    ucontext_t *cur_ctx, *next_ctx;

    cur_ctx = &(uthread_manager.uthreads[uthread_manager.uthread_cur]->uctx);
    uthread_manager.uthread_cur = (uthread_manager.uthread_cur + 1) % uthread_manager.uthread_count;
    next_ctx = &(uthread_manager.uthreads[uthread_manager.uthread_cur]->uctx);

    err = swapcontext(cur_ctx, next_ctx);
    if (err == -1) {
        perror("sheduler: swapcontext() failed:");
        exit(1);
    }
}

int uthread_create(uthread_t *uthread, void (*thread_func), void *arg) {
    static int uthread_id = 0;
    uthread_t new_uthread;
    void *stack;
    int err;

    uthread_id++; 

    printf("uthread_create: creating thread %d\n", uthread_id);

    stack = create_stack(STACK_SIZE, uthread_id);
    if (stack == NULL) {
        fprintf(stderr, "create_stack() failed\n");
        return -1;
    }
    new_uthread = (uthread_struct_t*) (stack + STACK_SIZE - sizeof(uthread_struct_t));

    err = getcontext(&new_uthread->uctx);
    if (err == -1) {
        perror("uthread_create: getcontext() failed:");
        return -1;
    }
    
    new_uthread->uctx.uc_stack.ss_sp = stack;
    new_uthread->uctx.uc_stack.ss_size = STACK_SIZE - sizeof(uthread_t);
    new_uthread->uctx.uc_link = &uthread_manager.uthreads[0]->uctx;

    makecontext(&new_uthread->uctx, (void (*)(void)) uthread_startup, 1, new_uthread);
    
    new_uthread->uthread_id = uthread_id;
    new_uthread->thread_func = thread_func;
    new_uthread->arg = arg;
    new_uthread->finished = 0;

    uthread_manager.uthreads[uthread_manager.uthread_count] = new_uthread;
    uthread_manager.uthread_count++;

    *uthread = new_uthread;

    return 0;
}

int thread_is_finished(uthread_t utid) {
    if (utid->finished == 1) {
        return 1;
    }
    return 0;
}

void init_thread(uthread_t thread) {
    uthread_manager.uthreads[uthread_manager.uthread_count] = thread;
    uthread_manager.uthread_count++;
}
