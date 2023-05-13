#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>

#define BUFFER_SIZE 4096
#define PAGE_SIZE 0x1000 // 4 кб

void stack(int it) {
    char buffer[BUFFER_SIZE];
    printf("Call %d: stack top is at %p\n", it, &buffer);
    ++it;
    usleep(5000);

    stack(it);
}

void heap() {
    int i = 1;
    char* buffer = NULL;
    while (1) {
        buffer = malloc(BUFFER_SIZE * 5);
        if (buffer == NULL) {
            perror("malloc");
            exit(1);
        }

        printf("Iteration %d: Allocated %d bytes at %p\n", i++, BUFFER_SIZE * 5, buffer);

        sleep(1); // ждем 1 секунду между итерациями
    }
    free(buffer);
}

void read_write_region() {
    char* region = mmap(NULL, PAGE_SIZE * 10, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (region == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    printf("Created a new region at: %p\n", region);
    sleep(10);

    // Изменяем права доступа к региону
    printf("Change protection of region\n");
    int result = mprotect(region, PAGE_SIZE * 10, PROT_NONE);
    if (result == -1) {
        perror("mprotect");
    }
    sleep(6);

    printf("Trying to read from region\n");
    printf("%c\n", *((char*)region));

    sleep(5);

    // Попытка записи в регион с запрещенной записью
    printf("Trying to write from region\n");
    //*((char*)region) = 'A';

    // Отсоединяем страницы 4-6 в созданном регионе
    printf("Disconnect 4-6 region\n");
    munmap(region + PAGE_SIZE * 4, PAGE_SIZE * 2);
    sleep(20);
    

    munmap(region, PAGE_SIZE * 4);
    munmap(region + PAGE_SIZE * 6, PAGE_SIZE * 4);
}

void signal_handler(int signum) {
    printf("Recieved signal %d\n", signum);
}

int main() {
    pid_t pid = getpid();
    printf("Process PID: %d\n", pid);

    sleep(15);
    // Перехватка сигнала
    if (signal(SIGSEGV, SIG_IGN) == SIG_ERR) {
        perror("signal");
    }

    //stack(1);
    
    //heap();

    read_write_region();

    return 0;
}
