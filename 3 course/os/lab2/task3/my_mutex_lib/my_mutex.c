#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <errno.h>

#include "my_mutex.h"

struct my_mutex {
    int lock;
};

static int my_futex(int *uaddr, int futex_op, int val,
        const struct timespec *timeout, int *auddr2, int val3) {
            return syscall(SYS_futex, uaddr, futex_op, val, timeout,
            uaddr, val3);
}

void my_mutex_init(my_mutex_t *m) {
    m->lock = 1;
}

void my_mutex_lock(my_mutex_t *m) {
    int err;

    while (1) {
        int one = 1;

        if (atomic_compare_exchange_strong(&m->lock, &one, 0)) {
            break;
        }

        err = my_futex(&m->lock, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (err == -1 && errno != EAGAIN) {
            printf("futex FUTEX_WAIT failed: %s\n", strerror(errno));
            abort();
        }
    }
}

void my_mutex_unlock(my_mutex_t *m) {
    const int zero = 0;
    if (atomic_compare_exchange_strong(&m->lock, &zero, 1)) {
        int err;

        err = my_futex(&m->lock, FUTEX_WAIT, 1, NULL, NULL, 0);
        if (err == -1) {
            printf("futex FUTEX_WAIT failed: %s\n", strerror(errno));
            abort();
        }
    }
}