#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
	printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());

	return (void *)42;              // Возвращение числа 42 в main поток
    //return (void *)"hello world";   // Возвращение указателя на строку в main
}

int main() {
	pthread_t tid;
	int err;
    void *result;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    // Дожидаемся завершения дополнительного потока
    pthread_join(tid, &result);


    printf("main: Child thread returned %d\n", (int)result);            // Вывод числа 42
    //printf("Main thread: Child thread returned: %s\n", (char *)result); // Вывод строки

	return 0;
}
