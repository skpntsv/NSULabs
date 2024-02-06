#define _GNU_SOURCE

#include "my_mutex.h"

void my_mutex_init(my_mutex_t* m) {
	m->lock = 1;
}

int futex(int* uaddr, int futex_op, int val,
	const struct timespec* timeout, int* uaddr2, int val3) {

	return syscall(SYS_futex, uaddr, futex_op, val,
		timeout, uaddr2, val3);
}

void my_mutex_lock(my_mutex_t* m) {
	int err;

	while (1) {
		int one = 1;
		if (atomic_compare_exchange_strong(&m->lock, &one, 0))
			break;

		err = futex(&m->lock, FUTEX_WAIT, 0, NULL, NULL, 0);
		if (err == -1 && errno != EAGAIN) {
			perror("futex WAIT");
			abort();
		}
	}
}

void my_mutex_unlock(my_mutex_t* m) {
	int zero = 0;
	if (atomic_compare_exchange_strong(&m->lock, &zero, 1)) {
		int err;

		err = futex(&m->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
		if (err == -1) {
			perror("futex WAIT");
			abort();
		}
	}
}