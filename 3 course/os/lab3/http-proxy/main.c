#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "network_utils.h"

#define PORT 80
#define LISTENQ 10
#define MAX_URL_SIZE 256
#define MAX_BUFFER_SIZE 1024*16
#define CACHE_SIZE 1024*4

typedef struct Request_t {
    char host[MAX_URL_SIZE];
    char path[MAX_URL_SIZE];
    int port;

} request;

typedef struct Cache_t {
    char url[MAX_URL_SIZE];
    char body[MAX_BUFFER_SIZE];

    struct cache *next;
} cache;

typedef struct ThreadArgs_t {
    int client_socket;
    // TODO добавить кэш
} threadArgs;

void parse_url(char *url, request *content) {
    char tmp[MAX_BUFFER_SIZE];
    if (strstr(url, "http://") != NULL) {
        sscanf(url, "http://%[^/]%s", tmp, content->path);
    } else {
        sscanf(url, "%[^/]%s", tmp, content->path);
    }

    if (strstr(tmp, ":") != NULL) {
        sscanf(tmp, "%[^:]:%d", content->host, &content->port);
    } else {
        strcpy(content->host, tmp);
        content->port = 80;
    }

    if (content->path[0]) {
        strcpy(content->path, "./");
    }
}

void* client_handler(void* args) {
    int client_socket = *(int*) args;

    char buffer[MAX_BUFFER_SIZE];
    
    int bytes_read = receive_request(client_socket, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        perror("receive_all");
        close(client_socket);

        return NULL;
    }
    printf("buffer =\n %s\n", buffer);

    char method[100], url[MAX_URL_SIZE], version[100];
    sscanf(buffer, "%s %s %s", method, url, version);

    if (strcasecmp(method, "GET")) {
        printf("This proxy doesn't support this method - %s\n", method);
    }

    request content;
    parse_url(url, &content);
    
    // Создаём новый HTTP запрос
    char new_request[MAX_BUFFER_SIZE];
    // snprintf(new_request, "GET %s HTTP/1.0\n", content.path);

    // snprintf(new_request, "%sHost: %s\n", new_request, content.host);

    int website_socket = create_socket(content.host, content.port);

    send_request(website_socket, buffer, sizeof(buffer));
    printf("Send request to website:\n %s\n", buffer);

    char response[MAX_BUFFER_SIZE];
    receive_request(website_socket, response, sizeof(response));
    printf("Get request from website:\n%s\n", response);

    close(website_socket);

    send_request(client_socket, response, sizeof(response));

    return NULL;
}

void start_proxy_server() {
    int server_socket, client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    init_server_socket(&server_socket, PORT, LISTENQ);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        pthread_t thread;
        pthread_create(&thread, NULL, client_handler, &client_socket);
        pthread_detach(thread);
    }

    close(server_socket);
}

int main() {
    start_proxy_server();

    return 0;
}