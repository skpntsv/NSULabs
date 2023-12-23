#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define STRING_LENGTH 1000

typedef struct Node_t {
    char* value;
    struct Node_t* next;
    pthread_rwlock_t sync;
} Node;

typedef struct Storage_t {
    Node* first;
} Storage;

typedef struct Cache_t {
    char* url;
    Storage* response;
    pthread_mutex_t mutex;

    struct Cache_t* next;
} Cache;

typedef struct Map_t {
    Cache* first;
} Map;

Map* map_init();
Storage* map_add(Map* map, const char* url);
Storage* map_find_by_url(Map* map, const char* url);

Storage* storage_init();
void storage_add(Storage* storage, const char* value);
void storage_print(Storage* storage);
void storage_destroy(Storage* storage);

#endif