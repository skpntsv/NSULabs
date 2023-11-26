#include <stdio.h>
#include <stdlib.h>

#include "linkedlist.h"

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

    newNode->next = storage->first;
    storage->first = newNode;
}

void fill_storage(Storage* storage, int num_nodes) {
    for (int i = 0; i < num_nodes; ++i) {
        char buff[10];
        sprintf(buff, "%d", i);
        add_node(storage, buff);
    }
}

void print_storage(Storage* storage) {
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

        pthread_mutex_destroy(&current->sync);
        
        free(current);
        current = next;
    }

    free(storage);
}