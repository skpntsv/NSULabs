#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <errno.h>

#include "my_mutex.h"

void my_mutex_init(my_mutex_t *mutex) { 
    mutex->lock = 0; 
}

void my_mutex_lock(my_mutex_t *mutex) {
    while (__sync_lock_test_and_set(&mutex->lock, 1) != 0) {
        syscall(SYS_futex, &mutex->lock, FUTEX_WAIT, 1, NULL, NULL, 0);
    }
}

void my_mutex_unlock(my_mutex_t *mutex) {
    __sync_lock_release(&mutex->lock);
    syscall(SYS_futex, &mutex->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
}