#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <semaphore.h>


#include "networks/network_utils.h"
#include "proxy.h"
#include "networks/http_message.h"
#include "cache/linkedlist.h"

#define MAX_THREADS 10
#define PORT 80
#define LISTENQ 10
#define DEFAULT_BUFFER_SIZE (1024*2)

typedef struct ThreadArgs_t {
    int client_socket;
    Map* cache;
    sem_t *thread_sem;
} threadArgs;

void *client_handler(void *args) {
    threadArgs *thread_args = (threadArgs *)args;
    int client_socket = thread_args->client_socket;
    Map *cache = thread_args->cache;
    sem_t *thread_sem = thread_args->thread_sem;

    int buffer_size = DEFAULT_BUFFER_SIZE;
    int k = 0;

    char *line;
    int website_socket;
    Cache* cache_node = NULL;
    Storage* cachedResponse = NULL;

    http_request *request = http_read_header(client_socket);
    if (request == NULL) {
        printf("Failed to parse the header\n");

        close(client_socket);
        return NULL;
    }
    // Работаем с КЕШОМ
    char* url = strdup(request->search_path);
    if (!url) {
        perror("strdup in url");
        free(url);
        abort();
    }
    printf("URL: %s\n", url);

    pthread_mutex_lock(&cache->mutex);
    cache_node = map_find_by_url(cache, url);

    if (cache_node == NULL) {
        // В кеше не нашлось данных
        printf("There is not in cache\n");
        website_socket = http_connect(request);
        if (website_socket == -1) {
            printf("Failed to connect to host\n");

            http_request_destroy(request);
            close(client_socket);
            return NULL;
        }
        http_request_send(website_socket, request);
        http_request_destroy(request);

        printf("Start to retrieve the response header\n");
        cache_node = map_add(cache, url);

        cachedResponse = cache_node->response;
        int line_length;
        Node* current = NULL;
        Node* prev = NULL;
        while (1) {
            line = read_line(website_socket);
            line_length = strlen(line);
            //printf("STRLEN() = %d\n", line_length);
            k++;
            //  printf("line: %s\n", line);
            current = storage_add(cachedResponse, line, line_length);
            //printf("storage_add: %s\n", current->value);


            int err = send_to_client(client_socket, line, 0, line_length);
            if (err == -1) {
                printf("Send to client headers end with ERROR\n");
                free(line);
                pthread_mutex_unlock(&cache->mutex);

                return NULL;
            }

            if (strstr(line, "Content-Length: ")) {
                size_t content_length = atoll(line + strlen("Content-Length: "));
                buffer_size = (content_length < DEFAULT_BUFFER_SIZE) ? content_length : DEFAULT_BUFFER_SIZE;
            }

            if (line[0] == '\r' && line[1] == '\n') {
                printf("get the end of the HTTP response header\n");
                free(line);
                break;
            }
            free(line);
        }
        pthread_mutex_unlock(&cache->mutex);  // разблокировали хедеры

        pthread_rwlock_wrlock(&current->sync);
        printf("Start to send BODY with buffer %d bytes in package\n", buffer_size);
        while (1) {
            prev = current;
            ssize_t body_length;
            char *body = http_read_body(website_socket, &body_length, buffer_size);
            if (body == NULL) {
                break;
            }
            int err = send_to_client(client_socket, body, 0, body_length);
            if (err == -1) {
                printf("Send to client body ended with ERROR\n");
                free(body);

                pthread_rwlock_unlock(&prev->sync);
                return NULL;
            }
            k++;
            current = storage_add(cachedResponse, body, body_length);
            pthread_rwlock_wrlock(&current->sync);
            pthread_rwlock_unlock(&prev->sync);
            free(body);
            //sleep(2);
        }
        pthread_rwlock_unlock(&prev->sync);

        printf("Send to client body was success\n");

        close(website_socket);
    } else {
        pthread_mutex_unlock(&cache->mutex);
        // Нашли в кеше
        printf("Found in cache, start to send it\n");
        cachedResponse = cache_node->response;

        Node* current = cachedResponse->first;
        k = 0;
        while (current != NULL) {
            pthread_rwlock_rdlock(&current->sync);
            k++;
            int err = send_to_client(client_socket, current->value, 0, current->size);
            if (err == -1) {
                printf("Send to client body ended with ERROR\n");
                pthread_rwlock_unlock(&current->sync);
                return NULL;
            }
            //printf("from cache: %s\n", current->value);
            //sleep(1);

            pthread_rwlock_unlock(&current->sync);
            current = current->next;
        }

        printf("Send to client data from cache success\n");
    }

    sem_post(thread_sem);

    //printf("K = %d\n", k);
    //free(url);
    close(client_socket);
    return NULL;
}

void *start_proxy_server(void *arg) {
    int server_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    Map* cache = map_init();

    sem_t thread_sem;
    sem_init(&thread_sem, 0, MAX_THREADS);

    init_server_socket(&server_socket, PORT, LISTENQ);

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        printf("Accepted a new connection\n");
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        sem_wait(&thread_sem);

        threadArgs *args = malloc(sizeof(threadArgs));
        args->client_socket = client_socket;
        args->cache = cache;
        args->thread_sem = &thread_sem;

        pthread_t thread;
        if (pthread_create(&thread, NULL, client_handler, args) != 0) {
            perror("Error creating thread");
            sem_post(&thread_sem);
            free(args);
            close(client_socket);
        } else {
            pthread_detach(thread);
        }
    }

    close(server_socket);
    sem_destroy(&thread_sem);
    return NULL;
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    pthread_t server_thread;

    if (pthread_create(&server_thread, NULL, start_proxy_server, NULL) != 0) {
        perror("Error creating server thread");
        exit(EXIT_FAILURE);
    }

    pthread_join(server_thread, NULL);

    return 0;
}