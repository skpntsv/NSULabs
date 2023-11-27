#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "linkedlist.h"

int ascending_counter = 0;
int descending_counter = 0;
int equal_counter = 0;

void* ascending_length_count(void* arg) {
    Storage* storage = (Storage *) arg;

    while (1) {
        int k = 0;
        Node* current = storage->first;

        while (current != NULL && current->next != NULL) {
            size_t currentLength = strlen(current->value);
            size_t nextLength = strlen(current->next->value);

            if (currentLength < nextLength) {
                k++;
            }

            current = current->next;
        }
        ascending_counter++;

        // printf("ascending_counter = %d\n", ascending_counter);
        // printf("k = %d\n", k);
    }

    return NULL;
}

void* descending_length_count(void* arg) {
    Storage* storage = (Storage *) arg;

    while (1) {
        int k = 0;
        Node* current = storage->first;

        while (current != NULL && current->next != NULL) {
            size_t currentLength = strlen(current->value);
            size_t nextLength = strlen(current->next->value);

            if (currentLength > nextLength) {
                k++;
            }

            current = current->next;
        }
        descending_counter++;

        // printf("descending_counter = %d\n", descending_counter);
        // printf("k = %d\n", k);
    }

    return NULL;
}

void* equal_length_сount(void* arg) {
    Storage* storage = (Storage *) arg;

    while (1) {
        int k = 0;

        Node* current = storage->first;

        while (current != NULL && current->next != NULL) {

            size_t currentLength = strlen(current->value);
            size_t nextLength = strlen(current->next->value);

            if (currentLength == nextLength) {
                k++;
            }

            current = current->next;
        }
        equal_counter++;

        // printf("equal_counter = %d\n", equal_counter);
        // printf("k = %d\n", k);
    }

    return NULL;
}

void *count_monitor(void *arg) {
    while (1) {
        printf("Ascending: %d, Descending: %d, Equal: %d\n", 
                    ascending_counter, descending_counter, equal_counter);
        sleep(1);
    }

    return NULL;
}

int main() {
    Storage* storage = storage_init();
    int err;

    storage_add(storage, "Hello");
    storage_add(storage, "World");
    fill_storage(storage, 1000);
    print_storage(storage);

    //sleep(15);
    pthread_t monitor;
    pthread_t equal_thread, ascending_thread, descending_thread;

    err = pthread_create(&equal_thread, NULL, equal_length_сount, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
	err = pthread_create(&ascending_thread, NULL, ascending_length_count, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
    err = pthread_create(&descending_thread, NULL, descending_length_count, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    err = pthread_create(&monitor, NULL, count_monitor, NULL);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	if (pthread_join(equal_thread, NULL)) {
		perror("pthread_join - equal_thread");
	}
	if (pthread_join(ascending_thread, NULL)) {
		perror("pthread_join - ascending_thread");
	}
    if (pthread_join(descending_thread, NULL)) {
		perror("pthread_join - descending_thread");
	}
    if (pthread_join(monitor, NULL)) {
		perror("pthread_join - monitor");
	}

    storage_destroy(storage);

    return 0;
}