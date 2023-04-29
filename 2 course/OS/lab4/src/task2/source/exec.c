#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv []) {
    
    printf("%d\n", getpid());
    sleep(1);
    execl("/proc/self/exe", "/proc/self/exe", NULL);
    printf("Hello world\n");

    return 0;
}