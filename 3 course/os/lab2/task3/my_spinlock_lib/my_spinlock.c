#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/syscall.h>

#include "my_spinlock.h"

struct my_spinlock {
    int lock;
};

void my_spinlock_init(my_spinlock_t *s) {
    s->lock = 1;
}

void my_spinlock_lock(my_spinlock_t *s) {
    while (1) {
        int one = 1;

        if (atomic_compare_exchange_strong(&s->lock, &one, 0)) {
            break;
        }
    }
}

void my_spinlock_unlock(my_spinlock_t *s) {
    const int zero = 0;
    atomic_compare_exchange_strong(&s->lock, &zero, 1);
}
