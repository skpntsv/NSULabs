#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int global_inited = 10;

int main() {

    int local_inited = 5;
    printf("global inited:  %p (%d)\n", &global_inited, global_inited);
    printf("local inited:   %p (%d)\n", &local_inited, local_inited);

    pid_t parent = getpid();
    printf("\npid: %d\n", parent);

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

        // Выводим адреса и значения переменных, созданных в родительском процессе
        printf("\tglobal inited:  %p (%d)\n", &global_inited, global_inited);
        printf("\tlocal inited:   %p (%d)\n", &local_inited, local_inited);

        // Меняем значения этих переменных и вывыодим это
        global_inited = 7;
        local_inited = 7;
        printf("\tChange num of vars\n");
        printf("\tglobal inited:  %p (%d)\n", &global_inited, global_inited);
        printf("\tlocal inited:   %p (%d)\n", &local_inited, local_inited);

        // Завершаем дочерний процесс
        sleep(10);

        // Генерируем ошибку сегментации
        int *ptr = NULL;
        *ptr = 123;
        
        exit(5);
    } else {    // Зашли в родительский процесс
        sleep(3);
        printf("Work in parent proccess:\n");

        // Выводим значения перменных в родительском процессе
        printf("global inited:  %p (%d)\n", &global_inited, global_inited);
        printf("local inited:   %p (%d)\n", &local_inited, local_inited);

        sleep(30);

        int status;
        pid_t terminated_pid = wait(&status); // ожидание завершения дочернего процесса

        if (terminated_pid == -1) {
            perror("wait");
        } else {
            if (WIFEXITED(status)) {
                printf("Дочерний процесс с PID %d завершился с кодом %d\n",
                    terminated_pid, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Дочерний процесс с PID %d завершился сигналом %d\n",
                    terminated_pid, WTERMSIG(status));
            }
        }
    }

    return 0;
}