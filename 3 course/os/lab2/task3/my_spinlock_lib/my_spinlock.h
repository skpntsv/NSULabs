#ifndef MY_SPINLOCK_H
#define MY_SPINLOCK_H

typedef struct my_spinlock my_spinlock_t;

void my_spinlock_init(my_spinlock_t *s);
void my_spinlock_lock(my_spinlock_t *s);
void my_spinlock_unlock(my_spinlock_t *s);

#endif