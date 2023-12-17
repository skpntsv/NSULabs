#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//#include "proxy.h"

#define PORT 80
#define LISTENQ 10
#define MAX_URL_SIZE 256
#define MAX_BUFFER_SIZE 1024*16
#define CACHE_SIZE 1024*4

typedef struct request_t {
    char host[MAX_URL_SIZE / 2];
    char path[MAX_URL_SIZE / 2];
    int port;

} request;

typedef struct cache_t {
    char url[MAX_URL_SIZE];
    char body[MAX_BUFFER_SIZE];

    struct cache *next;
} cache;

void error() {
    printf("something went wrong...");
}

void parse_url(char *url, request *content) {
    char tmp[MAX_BUFFER_SIZE];
    if (strstr(url, "http://") != NULL) {
        sscanf(url, "http://%[^/]%s", tmp, content->path);
    } // TODO else???

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
    
    int bytes_read = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
    if (bytes_read < 0) {
        perror("recv");
        close(client_socket);

        return NULL;
    }
    printf("buffer = %s\n", buffer);

    char method[100], url[MAX_URL_SIZE], version[100];
    sscanf(buffer, "%s %s %s", method, url, version);

    if (strcasecmp(method, "GET")) {
        printf("This proxy doesn't support this method - %s", method);
    }

    request content;
    parse_url(url, &content);
    
    // Создаём новый HTTP запрос
    char new_request[MAX_BUFFER_SIZE];
    // snprintf(new_request, "GET %s HTTP/1.0\n", content.path);

    // snprintf(new_request, "%sHost: %s\n", new_request, content.host);

    int clientfd;


    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }   

    if (listen(server_socket, LISTENQ) < 0) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Proxy server started and listening on port %d\n", PORT);

    while (1) {
        // Принимаем входящее соединение
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }
        //Создаем отдельный поток для обработки HTTP-запроса
        pthread_t thread;
        pthread_create(&thread, NULL, client_handler, &client_socket);
        pthread_detach(thread);

        //handle_http_request(client_socket);
    }

    close(server_socket);


    return 0;
}