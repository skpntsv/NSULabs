#ifndef UTHREAD_H
#define UTHREAD_H

#include <ucontext.h>

#define MAX_THREADS 10

typedef struct _uthread_struct_t {
    int              uthread_id;
    void             (*thread_func)(void*);
    void             *arg;
    void             *retval;

    volatile int     finished;
    ucontext_t       uctx;
} uthread_struct_t;

typedef uthread_struct_t *uthread_t;

typedef struct _uthread_manager_t {
    uthread_struct_t *uthreads[MAX_THREADS];
    int uthread_count;
    int uthread_cur;
} uthread_manager_t;

extern uthread_manager_t uthread_manager; // Declare the uthread_manager as external

int uthread_create(uthread_t *uthread, void (*thread_func)(void *), void *arg);
void uthread_sheduler(void);
int thread_is_finished(uthread_t utid);
void init_thread(uthread_t thread);

#endif
