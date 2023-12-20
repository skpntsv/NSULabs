#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/syscall.h>

typedef struct {
	int lock;
} my_mutex_t;

void my_mutex_init(my_mutex_t* m);

int futex(int* uaddr, int futex_op, int val,
	const struct timespec* timeout, int* uaddr2, int val3);

void my_mutex_lock(my_mutex_t* m);

void my_mutex_unlock(my_mutex_t* m);
