#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"

Map* map_init() {
    Map* map = (Map*)malloc(sizeof(Map));
    if (!map) {
        perror("malloc in map_init()");
        abort();
    }

    map->first = NULL;

    return map;
}

Storage* storage_init() {
    Storage* storage = (Storage*)malloc(sizeof(Storage));
    if (!storage) {
        perror("malloc in storage_init()");
        abort();
    }
    storage->first = NULL;

    return storage;
}

Cache* map_add(Map* map, const char* url) {
    Cache* newCache = (Cache*)malloc(sizeof(Cache));
    if (!newCache) {
        perror("malloc in map_add()");
        abort();
    }

    newCache->url = strdup(url);
    if (!newCache->url) {
        perror("strdup in map_add()");
        free(newCache);
        abort();
    }

    if (pthread_mutex_init(&newCache->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        abort();
    }

    newCache->response = storage_init();

    newCache->next = map->first;
    map->first = newCache;

    return map->first;
}

Cache* map_find_by_url(Map* map, const char* url) {
    Cache* current = map->first;
    while (current != NULL) {
        pthread_mutex_lock(&current->mutex);
        if (strcmp(current->url, url) == 0) {
            pthread_mutex_unlock(&current->mutex);
            return current;
        }
        printf("URL IN CACHE: %s\n", current->url);
        pthread_mutex_unlock(&current->mutex);
        current = current->next;
    }
    return NULL;
}

Node* storage_add(Storage* storage, const char* value, ssize_t length) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("malloc in push()");
        abort();
    }

    newNode->value = (char*)malloc(length);
    if (!newNode->value) {
        perror("malloc in storage_add()");
        free(newNode);
        abort();
    }

    memcpy(newNode->value, value, length);

    if (pthread_rwlock_init(&newNode->sync, NULL) != 0) {
        perror("pthread_rwlock_init");
        abort();
    }

//    newNode->next = storage->first;
//    storage->first = newNode;


    // Добавляем новый элемент в конец списка
    newNode->next = NULL;
    if (storage->first == NULL) {
        storage->first = newNode;
    } else {
        Node* current = storage->first;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }

    return newNode;
}

void storage_print(Storage* storage) {
    Node* current = storage->first;
    while (current != NULL) {
        printf("%s\n", current->value);
        current = current->next;
    }
    printf("\n");
}

void storage_destroy(Storage* storage) {
    Node* current = storage->first;
    while (current != NULL) {
        Node* next = current->next;

        pthread_rwlock_destroy(&current->sync);

        free(current);
        current = next;
    }

    free(storage);
}