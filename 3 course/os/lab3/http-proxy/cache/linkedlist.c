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

Storage* map_add(Map* map, const char* url) {
    Cache* newCache = (Cache*)malloc(sizeof(Cache));
    if (!newCache) {
        fprintf(stderr, "malloc in map_add() failed\n");
        abort();
    }

    newCache->url = strdup(url);
    if (!newCache->url) {
        perror("strdup in map_add()");
        free(newCache);
        abort();
    }
    printf("\n\n NEWCACHE = %s\n", url);

    if (pthread_mutex_init(&newCache->mutex, NULL) != 0) {
        perror("pthread_rwlock_init");
        abort();
    }

    newCache->response = storage_init();

    newCache->next = map->first;
    map->first = newCache;

    return map->first->response;
}

Storage* map_find_by_url(Map* map, const char* url) {
    Cache* current = map->first;
    while (current != NULL) {
        if (strcmp(current->url, url) == 0) {
            printf("TRUE\n");
            printf("TRUE\n");
            printf("TRUE\n");
            printf("TRUE\n");
            return current->response;
        }
        printf("\n\nFALSE: URL = %s\n\n", current->url);
        current = current->next;
    }
    return NULL;
}

void storage_add(Storage* storage, const char* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("malloc in push()");
        abort();
    }

    newNode->value = strdup(value);
    if (!newNode->value) {
        perror("strdup in storage_add()");
        free(newNode);
        abort();
    }

    if (pthread_rwlock_init(&newNode->sync, NULL) != 0) {
        perror("pthread_rwlock_init");
        abort();
    }

    newNode->next = storage->first;
    storage->first = newNode;
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