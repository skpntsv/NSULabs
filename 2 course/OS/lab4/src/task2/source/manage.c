#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

void stack(int it) {
    char buffer[BUFFER_SIZE];
    printf("Call %d: stack top is at %p\n", it, &buffer);
    ++it;
    usleep(5000);

    stack(it);
}

void heap() {
    int i = 1;
    char buffer = NULL;
    while (1) {
        buffer = malloc(BUFFER_SIZE);
        if (buffer == NULL) {
            perror("malloc");
            exit(1);
        }

        printf("Iteration %d: Allocated %d bytes at address %p\n", i++, BUFFER_SIZE, buffer);

        sleep(1); // ждем 1 секунду между итерациями

        free(buffer);
    }
}

int main() {
    pid_t pid = getpid();
    printf("Process PID: %d\n", pid);

    sleep(10);

    stack(1);
    
    heap();

    return 0;
}
