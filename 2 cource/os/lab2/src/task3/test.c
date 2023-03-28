#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <sys/syscall.h>

int main() {
    pid_t child_pid;
    long orig_eax, eax;
    int status;
    struct user_regs_struct regs;

    child_pid = fork();
    if(child_pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    } else {
        while(1) {
            wait(&status);
            if(WIFEXITED(status))
                break;
            orig_eax = ptrace(PTRACE_PEEKUSER, child_pid, sizeof(long) * ORIG_RAX, NULL);
            if(orig_eax == SYS_write) {
                ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
                printf("write called with fd=%lld, buf=%lld, count=%lld\n",
                    regs.rdi, regs.rsi, regs.rdx);
            }
            ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
        }
    }

    return 0;
}
