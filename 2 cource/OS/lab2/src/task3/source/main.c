#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <sys/user.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Используйте: %s <команда>\n", argv[0]);
        exit(1);
    }
    pid_t child;
    long orig_rax;
    int status;
    int flag = 1;
    //struct user_regs_struct regs;

    child = fork();
    if (child < 0) {
        perror("fork");
        exit(1);
    }

    if (child == 0) {
        // Дочерний процесс
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
            perror("ptrace - PTRACE_TRACEME");
            exit(1);
        }  //сообщаем родительному процессу что мы хотим быть отслеживаемым

        char *args[argc];
        args[0] = argv[1];
        for (int i = 2; i < argc; ++i) {
            args[i - 1] = argv[i];
        }
        args[argc - 1] = NULL;

        execvp(argv[1], args);
        perror("execvp");
        exit(1);
    } else {
        // Родительский процесс
        if (waitpid(child, &status, 0) == -1) {
            perror("waitpid");
            exit(1);
        }
        
        orig_rax = ptrace(PTRACE_PEEKUSER, child, sizeof(long) * ORIG_RAX, NULL);
        if (orig_rax == -1) {
            perror("ptrace - PTRACE_PEEKUSER");
            exit(1);
        }

        printf("System call %ld\n", orig_rax);        

        while (WIFSTOPPED(status)) {
            //ptrace(PTRACE_GETREGS, child, NULL, &regs);
            if (!flag) {
                orig_rax = ptrace(PTRACE_PEEKUSER, child, sizeof(long) * ORIG_RAX, NULL);
                if (orig_rax == -1) {
                    perror("ptrace - PTRACE_PEEKUSER");
                    exit(1);
                }

                printf("System call %ld\n", orig_rax);
                flag = 1;
            }
            else {
                flag = 0;
            }
            if (ptrace(PTRACE_SYSCALL, child, NULL, NULL) != 0) {
                perror("ptrace - PTRACE_SYSCALL");
                exit(1);
            }
            if (waitpid(child, &status, 0) == -1) {
                perror("waitpid");
                exit(1);
            }
        }
    }

    return 0;
}
