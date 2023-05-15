#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_inited = 10;

int main() {

    int local_inited = 5;
    printf("global inined:  %p (%d)\n", &global_inited, global_inited);
    printf("local inited:   %p (%d)\n", &local_inited, local_inited);

    pid_t parent = getpid();
    printf("\npid: %d\n", parent);

    pid_t child;
    child = fork();
    if (child < -1) {
        perror("fork");
        exit(1);
    }
    // Работает в дочернем процессе
    if (child == 0) {
        printf("Work in child proccess:\n");
        // Выводим pid родительского и дочернего процесса
        printf("\tchild pid:  %d\n", getpid());
        printf("\tparent pid: %d\n", getppid());

        // Выводим адреса и значения переменных, созданных в родительском процессе
        printf("\tglobal inined:  %p (%d)\n", &global_inited, global_inited);
        printf("\tlocal inited:   %p (%d)\n", &local_inited, local_inited);

        // Меняем значения этих переменных и вывыодим это
        global_inited = 7;
        local_inited = 7;
        printf("\tChange num of vars\n");
        printf("\tglobal inined:  %p (%d)\n", &global_inited, global_inited);
        printf("\tlocal inited:   %p (%d)\n", &local_inited, local_inited);

        // Завершаем дочерний процесс
        exit(5);
    } else {    // Зашли в родительский процесс
        sleep(3);
        printf("Work in parent proccess:\n");

        // Выводим значения перменных в родительском процессе
        printf("global inined:  %p (%d)\n", &global_inited, global_inited);
        printf("local inited:   %p (%d)\n", &local_inited, local_inited);

        sleep(30);
    }

    

    return 0;
}