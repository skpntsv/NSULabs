#include <stdio.h>
#include <stdlib.h>

#include <linkedlist.h>

Storage storage;
Counter ascendingCounter = {0, PTHREAD_MUTEX_INITIALIZER};
Counter descendingCounter = {0, PTHREAD_MUTEX_INITIALIZER};
Counter equalCounter = {0, PTHREAD_MUTEX_INITIALIZER};

void initializeStorage(Storage* storage) {
    storage->first = NULL;
}

void addNode(Storage* storage, const char* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&newNode->sync, NULL);

    snprintf(newNode->value, MAX_STRING_LENGTH, "%s", value);
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