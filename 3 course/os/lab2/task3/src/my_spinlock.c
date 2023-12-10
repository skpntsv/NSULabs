#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/syscall.h>

typedef struct {
    int lock;
} spinlock_t;

void spinlock_init(spinlock_t *s) {
    s->lock = 1;
}

void spinlock_lock(spinlock_t *s) {
    while (1) {
        int one = 1;

        if (atomic_compare_exchange_strong(&s->lock, &one, 0)) {
            break;
        }
    }
}

void spinlock_unlock(spinlock_t *s) {
    const int zero = 0;
    atomic_compare_exchange_strong(&s->lock, &zero, 1);
}