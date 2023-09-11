#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global_num = 1;

void *changer_thread(void *arg) {
	int local_num = 5;
	int static_local_num;
	int const const_local_num;

	printf("mythread [%d %d %d %ld]: Hello from mythread!\n\n", getpid(), getppid(), gettid(), pthread_self());

	// Выводим адреса разных переменных
	printf("global_num: %d [%p]\n", global_num, &global_num);
	printf("local_num: %d [%p]\n\n", local_num, &local_num);
	printf("static_local_num: %p\n", &static_local_num);
	printf("const_local_num: %p\n\n", &const_local_num);

	// Изменяем адреса в потоке
	global_num++;
	local_num += 2;
	printf("global_num: %d [%p]\n", global_num, &global_num);
	printf("local_num: %d [%p]\n\n", local_num, &local_num);

	sleep(10);
	
	sleep(100); // для /proc/pid/maps
	return NULL;
}

void *reader_thread(void *arg) {
	int local_num = 5;
	int static_local_num;
	int const const_local_num;

	sleep(5);

	printf("mythread [%d %d %d %ld]: Hello from mythread!\n\n", getpid(), getppid(), gettid(), pthread_self());

	// Выводим адреса разных переменных
	printf("global_num: %d [%p]\n", global_num, &global_num);
	printf("local_num: %d [%p]\n\n", local_num, &local_num);
	printf("static_local_num: %p\n", &static_local_num);
	printf("const_local_num: %p\n\n", &const_local_num);

	sleep(100); // для /proc/pid/maps
	return NULL;
}

int main() {
	pthread_t tid;
	int err;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	for (size_t i = 0; i < 5; ++i) {
		if (i < 2) {
			err = pthread_create(&tid, NULL, changer_thread, NULL);
		}
		else {
			err = pthread_create(&tid, NULL, reader_thread, NULL);
		}
		if (err) {
			printf("main: pthread_create() failed: %s\n", strerror(err));
			return -1;
		}
	}

	sleep(100);
	pthread_exit(NULL);
	return 0;
}

