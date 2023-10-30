#ifndef MYTHREAD_H
#define MYTHREAD_H

#define PAGE 4096
#define STACK_SIZE (PAGE * 5)

typedef void *(*start_routine_t)(void*);

typedef struct _mythread_t {
    int              id;
    start_routine_t  start_routine;
    void             *arg;
    void             *retval;
    void             *stack;

    volatile int     joined;
    volatile int     finished;
} mythread_struct_t;

typedef mythread_struct_t *mythread_t;

int mythread_startup(void *arg);
void *create_stack(off_t size);
int mythread_create(mythread_t *mytid, void *(start_routine) (void *), void *arg);
int mythread_join(mythread_t mytid, void **retval);
int getmypid(mythread_t mytid);

#endif