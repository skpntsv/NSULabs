#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

    pid_t child;
    child = fork();
    if (child == -1) {
        perror("fork");
        exit(1);
    }
    // Работает в дочернем процессе
    if (child == 0) {
        printf("Work in child proccess:\n");
        // Выводим pid родительского и дочернего процесса
        printf("\tchild pid:  %d\n", getpid());
        printf("\tparent pid: %d\n", getppid());

        // Завершаем дочерний процесс
        sleep(15);
        exit(5);
    } else {    // Зашли в родительский процесс
        sleep(3);
        printf("Work in parent proccess:\n");

        pid_t parent = getpid();
        printf("\npid: %d\n", parent);

        exit(0);
    }

    return 0;
}