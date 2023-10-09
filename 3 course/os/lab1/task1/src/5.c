#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


void *thread_1(void* arg){
	sigset_t set;
    if (sigfillset(&set) == -1) {
        perror("thread_1: sigfillset() failed");
        return NULL;
    }
	if (pthread_sigmask(SIG_BLOCK, &set, NULL)) {
        perror("thread_1: pthread_sigmask() failed");
        return NULL;
    }

	printf("thread_1: first thread blocked signals\n");

	sleep(100);
	printf("thread_1: first thread has finished its work\n");
	return NULL;
}

void sigint_handler(int sig){
	printf("Signal received: %d\n", sig);
}

void *thread_2(void * arg){
	signal(SIGINT, sigint_handler);

	return NULL;
}

void *thread_3(void * arg){
	int sig;
	
	sigset_t set;
	if (sigemptyset(&set) == -1) {
        perror("thread_3: sigemptyset() failed");
        return NULL;
    }
	if (sigaddset(&set, SIGQUIT) == -1) {
        perror("thread_3: sigaddset failed");
        return NULL;
    }

	pthread_sigset(SIG_BLOCK, &set, NULL);
	
	printf("thread_3: third thread is waitng a SIGQUIT signal\n");
	
	sigwait(&set,&sig);
	
	printf("thread_3: third thread received SIGQUIT signal: %d\n",sig);

	return NULL;
}


int main(){
    pthread_t tid1; 
    pthread_t tid2;
    pthread_t tid3;

	printf("Pid:%d\n",getpid());

    pthread_create(&tid1, NULL, thread_1, NULL);	
    pthread_create(&tid2, NULL, thread_2, NULL);
    pthread_create(&tid3, NULL, thread_3, NULL);

   pthread_kill(tid1,SIGINT);
   sleep(2);
   pthread_kill(tid2,SIGINT);
   sleep(2);
   pthread_kill(tid3,SIGQUIT);
   sleep(2);

   // останавливать с помощью консоли (на клавиатуре)


    pthread_exit(0);
    return 0;
}