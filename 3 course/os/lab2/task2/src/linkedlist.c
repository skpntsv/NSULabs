#include <stdio.h>
#include <stdlib.h>

#include "linkedlist.h"

Storage storage;
Counter ascendingCounter = {0, PTHREAD_MUTEX_INITIALIZER};
Counter descendingCounter = {0, PTHREAD_MUTEX_INITIALIZER};
Counter equalCounter = {0, PTHREAD_MUTEX_INITIALIZER};

Storage* storage_init() {
    Storage* storage = (Storage*)malloc(sizeof(Storage));
    if (!storage) {
        perror("malloc in storage_init()");
        abort();
    }
    storage->first = NULL;

    return storage;
}

void storage_add(Storage* storage, const char* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("malloc in push()");
        abort();
    }
    snprintf(newNode->value, STRING_LENGTH, "%s", value);

	if (pthread_mutex_init(&newNode->sync, NULL) != 0) {
		perror("pthread_mutex_init");
		abort();
	}
    pthread_mutex_lock(&newNode->sync);

    newNode->next = storage->first;
    storage->first = newNode;
}

void printList(Storage* storage) {
    Node* current = storage->first;
    while (current != NULL) {
        printf("%s\n", current->value);
        current = current->next;
    }
}

void storage_destroy(Storage* storage) {
    Node* current = storage->first;
    while (current != NULL) {
        Node* next = current->next;

        pthread_mutex_destroy(&current->sync);
        
        free(current);
        current = next;
    }

    free(storage);
}