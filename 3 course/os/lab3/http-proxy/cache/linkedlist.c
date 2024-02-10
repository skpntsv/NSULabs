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

    if (pthread_mutex_init(&map->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        abort();
    }

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

    size_t length = strlen(url) + 1;
    newCache->url = (char*)malloc(length);
    if (!newCache->url) {
        perror("malloc in storage_add()");
        abort();
    }
    memcpy(newCache->url, url, length);
    newCache->url[++length] = '\0';

    newCache->response = storage_init();

    newCache->next = map->first;
    map->first = newCache;

    return map->first;
}

Cache* map_find_by_url(Map* map, const char* url) {
    Cache* current = map->first;
    while (current != NULL) {
        if (strcmp(current->url, url) == 0) {
            return current;
        }
        printf("URL IN CACHE: %s\n", current->url);
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

    newNode->size = length;
    newNode->value = (char*)malloc(length);
    if (!newNode->value) {
        perror("malloc in storage_add()");
        free(newNode);
        abort();
    }

    memcpy(newNode->value, value, length);
    newNode->value[++length] = '\0';

    if (pthread_rwlock_init(&newNode->sync, NULL) != 0) {
        perror("pthread_rwlock_init");
        abort();
    }

    newNode->next = NULL;
    // Если список пуст, устанавливаем и first, и last на новый узел
    if (storage->first == NULL) {
        storage->first = newNode;
        storage->last = newNode;
    } else {
        // Иначе, добавляем новый узел в конец списка
        storage->last->next = newNode;
        storage->last = newNode;
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