#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "linkedlist.h"

Counter ascendingCounter = {0, PTHREAD_MUTEX_INITIALIZER};
Counter descendingCounter = {0, PTHREAD_MUTEX_INITIALIZER};
Counter equalCounter = {0, PTHREAD_MUTEX_INITIALIZER};

void* ascending_length_count(void* arg) {
    Storage* storage = (Storage *) args

    while (1) {
        int count = 0;

        Node* current = storage.first;

        while (current != NULL) {
            pthread_mutex_lock(&current->sync);
            Node* next = current->next;
            if (next != NULL && strlen(current->value) > strlen(next->value)) {
                count++;
            }
            pthread_mutex_unlock(&current->sync);
            current = next;
        }

        // Increment global variable or perform other actions as needed
        ascendingCounter.count++;

        // Sleep or other synchronization mechanism may be required
        sleep(1);
    }

    return NULL;
}

void* equalLengthCount(void* args) {
    Storage* storage = (Storage *) args

    while (1) {
        int k = 0;
        pthread_mutex_lock(&equalCounter.sync);

        Node* current = storage.first;

        while (current != NULL && current->next != NULL) {
            pthread_mutex_lock(&current->sync);
            pthread_mutex_lock(&current->next->sync);

            size_t currentLength = strlen(current->value);
            size_t nextLength = strlen(current->next->value);

            if (currentLength == nextLength) {
                k++;
            }

            pthread_mutex_unlock(&current->sync);
            pthread_mutex_unlock(&current->next->sync);

            current = current->next;
        }

        equalCounter.count++;

        pthread_mutex_unlock(&equalCounter.sync);
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
    int iteration_count1 = 0, iteration_count2 = 0, iteration_count3 = 0;

    ThreadData thread_data1 = {storage, &iteration_count1};
    ThreadData thread_data2 = {storage, &iteration_count2};
    ThreadData thread_data3 = {storage, &iteration_count3};

    pthread_create(&thread1, NULL, thread_function, &thread_data1);
    pthread_create(&thread2, NULL, thread_function, &thread_data2);
    pthread_create(&thread3, NULL, thread_function, &thread_data3);

    // Sleep or join threads as needed
    sleep(5); // Run for 5 seconds, for example

    // Cleanup and join threads
    pthread_cancel(thread1);
    pthread_cancel(thread2);
    pthread_cancel(thread3);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    // Cleanup storage
    storage_destroy(storage);

    return 0;
}