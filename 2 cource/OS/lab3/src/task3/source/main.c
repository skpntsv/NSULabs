#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define PAGE_SIZE 0x1000 // 4 кб (размер вирт памяти)

void printPagemap(uint64_t start, uint64_t end, char* path) {
    char pagemapPath[256];
    sprintf(pagemapPath, "%s/pagemap", path);

    int fd = open(pagemapPath, O_RDONLY);
    if (fd < 0) {
        perror("failed to open pagemap file");
        exit(1);
    }
    uint64_t data;
    uint64_t i;
    for(i = start; i < end; i += PAGE_SIZE) {
        uint64_t index = (i / PAGE_SIZE) * sizeof(data);
        if(pread(fd, &data, sizeof(data), index) != sizeof(data)) {
            perror("pread");
            exit(1);
        }
    
        // i = адрес страницы в виртуальном адресном пространстве процесса, отображаемой на физическую страницу
        uint64_t page_frame_number = data & 0x7fffffffffffff;   // номер физической страницы, на которую отображается виртуальная страница
        uint64_t page_exclusive_mapped = (data >> 56) & 1;      // страница памяти находится в единоличном использовании процесса и не совместно используется с другими процессами.
        uint64_t page_soft_dirty = (data >> 55) & 1;            // установлен, если страница была изменена, но не была записана обратно на диск.
        uint64_t page_swap = (data >> 62) & 1;                  // установлен, если страница была выгружена на диск, чтобы освободить физическую память.
        uint64_t page_present = (data >> 63) & 1;               // установлен, если страница присутствует в физической памяти.
        uint64_t page_file_shared = (data >> 61) & 1;           // бит, указывающий, является ли страница отображением файла или является общей страницей, используемой несколькими процессами

        printf("0x%-11lx: pfn: %-9lx soft-dirty: %ld file/shared: %ld "
           "swapped: %ld present: %ld exclusive_mapped: %ld\n",
           i,
           page_frame_number,
           page_soft_dirty,
           page_file_shared,
            page_swap,
            page_present,
            page_exclusive_mapped);

    }
    close(fd);
}

void readMaps(char* path) {
    char mapsPath[256];
    sprintf(mapsPath, "%s/maps", path);

    FILE* file = fopen(mapsPath, "r");
    if (file == NULL) {
        perror("failed to open maps file");
        exit(1);
    }
    char* line = NULL;
    size_t lineSize = 0;
    size_t readSize = 0;

    while ((readSize = getline(&line, &lineSize, file)) != -1) {
        uint64_t start, end;
        char perms[5];

        sscanf(line, "%lx-%lx %s", &start, &end, perms);
        printPagemap(start, end, path);
    }

    free(line);
    fclose(file);
}

int main(int argc, char *argv[]) {
    char path[128];

    if (argc != 2) {
        printf("Введите pid\n");
        exit(1);
    }
    sprintf(path, "/proc/%s", argv[1]);
    
    readMaps(path);
    

    return 0;
}