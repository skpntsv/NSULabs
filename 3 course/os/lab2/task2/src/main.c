#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "linkedlist.h"

void* thread_function(void* arg) {
    Storage* storage = (Storage*)arg;

    while (1) {
        // Your logic for searching and counting pairs goes here

        // Example: counting pairs with the same length
        int count = 0;
        Node* current = storage->first;
        while (current != NULL) {
            pthread_mutex_lock(&current->sync);
            Node* next = current->next;
            if (next != NULL && strlen(current->value) == strlen(next->value)) {
                count++;
            }
            pthread_mutex_unlock(&current->sync);
            current = next;
        }
        break;
        // Increment global variable or perform other actions as needed

        // Sleep or other synchronization mechanism may be required
    }

    return NULL;
}

int main() {
    Storage* storage = storage_init();

    // Example: Adding strings to the storage
    storage_add(storage, "Hello");
    storage_add(storage, "World");
    // Add more strings as needed

    // Create and launch three threads using pthread_create
    pthread_t thread1, thread2, thread3;
    pthread_create(&thread1, NULL, thread_function, storage);
    pthread_create(&thread2, NULL, thread_function, storage);
    pthread_create(&thread3, NULL, thread_function, storage);

    // Sleep or join threads as needed

    // Cleanup and join threads
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    printList(storage);
    // Cleanup storage
    storage_destroy(storage);

    return 0;
}