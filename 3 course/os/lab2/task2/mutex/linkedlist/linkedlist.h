#ifndef LINKED_LIST_H
#define LINKED_LIST_H

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

Storage* storage_init();
void storage_add(Storage* storage, const char* value);
void storage_print(Storage* storage);
void storage_destroy(Storage* storage);

void swap_nodes(Node* prev, Node* current, Node* next);

#endif