#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sched.h>
#include <fcntl.h>

#define PAGE 4096
#define STACK_SIZE (PAGE * 8)

typedef struct Mythread_t {
    int             id;

    volatile int    finished;
    volatile int    joined;
} mythread_t;

int mythread_startup(void *arg) {

    arg->mythread_func(...);

}

void *create_stack(off_t size, int thread_num) {
    int stack_fd;
    void* stack;

    char* stack_file;
    snprintf(stack_file , sizeof(stack_file), "stack-%d", thread_num);

    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    ftruncate(stack_fd, 0);
    ftruncate(stack_fd, size);

    stack = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, stack_fd, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");

        return -1;
    }
    close(stack_fd);

    memset(stack, 0x7f, size);

    return stack;
}

int mythread_create(mythread_t thread, void *(start_routine), void *arg) {
    static int thread_num = 0;

    thread_num++; 

    int child_stack = create_stack(STACK_SIZE, thread_num);

    int child_pid = clone(mythread_startup, child_stack + STACK_SIZE, CLONE_VM | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND | SIGCHLD, NULL);
    if (child_pid == -1) {
        perror("clone");

        return -1;
    }
}

void *mythread(void *arg) {
    char *str = (char *)arg;

	for (int i = 1; i < 5; i++) {
        printf("mythread: %s\n", str);
        sleep(1);
    }
}

int main() {
    mythread_t tid;

    mythread_create(tid, mythread, "hello from main");

	return 0;
}
