#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define STRING_LENGTH 1000

typedef struct Node_t {
    char* value;
    ssize_t size;
    struct Node_t* next;
    pthread_rwlock_t sync;
} Node;

typedef struct Storage_t {
    Node* first;
    Node* last;
} Storage;

typedef struct Cache_t {
    char* url;
    Storage* response;

    struct Cache_t* next;
} Cache;

typedef struct Map_t {
    Cache* first;

    pthread_mutex_t mutex;
} Map;

Map* map_init();
Cache* map_add(Map* map, const char* url);
Cache* map_find_by_url(Map* map, const char* url);

Storage* storage_init();
Node* storage_add(Storage* storage, const char* value, ssize_t length);
void storage_print(Storage* storage);
void storage_destroy(Storage* storage);

#endif