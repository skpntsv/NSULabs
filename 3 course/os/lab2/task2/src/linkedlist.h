#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define STRING_LENGTH 100

typedef struct _Node {
    char value[STRING_LENGTH];
    struct _Node* next;
    pthread_mutex_t sync;
} Node;

typedef struct _Storage {
    Node* first;
} Storage;

typedef struct _Counter {
    int count;
    pthread_mutex_t sync;
} Counter;

Storage* storage_init();
void storage_add(Storage* storage, const char* value);
void printList(Storage* storage);
void storage_destroy(Storage* storage);

// void* ascendingLengthCount(void* args);
// void* descendingLengthCount(void* args);
// void* equalLengthCount(void* args);

// void* swapElements(void* args);