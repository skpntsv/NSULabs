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
#include <signal.h>

#include "network_utils.h"
#include "proxy.h"
#include "http_message.h"

#define PORT 80
#define LISTENQ 10
#define MAX_BUFFER_SIZE (1024*2)

typedef struct ThreadArgs_t {
    int client_socket;
    // TODO добавить кэш
} threadArgs;


char *read_line(int socket) {
    int buffer_size = 2;
    char *line = (char*)malloc(sizeof(char) * buffer_size + 1);
    char c;
    ssize_t length = 0;
    int counter = 0;

    while(1) {
        length = recv(socket, &c, 1, 0);
        if (length == -1) {
            perror("recv");
        }
        line[counter++] = c;

        if (c == '\n') {
            line[counter] = '\0';
            return line;
        }

        if (counter == buffer_size) {
            buffer_size *= 2;
            line = (char*)realloc(line, sizeof(char) * buffer_size);
        }
    }

    return NULL;
}

//void parse_url(char *url, request *content) {
//    char tmp[MAX_BUFFER_SIZE];
//    if (strstr(url, "http://") != NULL) {
//        sscanf(url, "http://%[^/]%s", tmp, content->path);
//    } else {
//        sscanf(url, "%[^/]%s", tmp, content->path);
//    }
//
//    if (strstr(tmp, ":") != NULL) {
//        sscanf(tmp, "%[^:]:%d", content->host, &content->port);
//    } else {
//        strcpy(content->host, tmp);
//        content->port = 80;
//    }
//
//    if (content->path[0]) {
//        strcpy(content->path, "./");
//    }
//
//    printf("CONTENT PATH: %s\n", content->path);
//}

int http_request_send(int socket, http_request *request) {
    char *request_buffer = http_build_request(request);
    if (request_buffer == NULL) {
        printf("new message is null\n");
        return -1;
    }
    printf("%p\n", request_buffer);
    printf("%s\n", request_buffer);
    if (send(socket, request_buffer, strlen(request_buffer), 0) == -1) {
        free(request_buffer);
        perror("send");
        return 1;
    }
    free(request_buffer);

    printf("Sent HTTP header to web server\n");

    return 0;
}

int send_to_client(int client_socket, char data[], int packages_size, ssize_t length) {
    if (length <= 0) {
        return 0;
    }

    int bytes_sent;
    if (packages_size <= 0) {
        bytes_sent = send(client_socket, data, length, 0);
    } else {
        int p;
        for (p = 0; p * packages_size + packages_size < length; p++) {
            bytes_sent = send(client_socket, (data + p * packages_size), packages_size, 0);
            if (bytes_sent == -1) {
                perror("Couldn't send data to the client (loop)");
                return -1;
            }
        }

        if (p * packages_size < length) {
            bytes_sent = send(client_socket, (data + p * packages_size), length - p * packages_size, 0);
        }
    }

    if (bytes_sent == -1) {
        perror("Couldn't send data to the client.");
        return -1;
    }

    return 0;
}


void *client_handler(void *args) {
    threadArgs *thread_args = (threadArgs *)args;
    int client_socket = thread_args->client_socket;

    char *line;
    int website_socket;

    http_request *request = http_read_header(client_socket);
    if (request == NULL) {
        printf("Failed to parse the header\n");

        close(client_socket);
        return NULL;
    }

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
    int line_length;
    while (1) {
        line = read_line(website_socket);
        line_length = strlen(line);
        int err = send_to_client(client_socket, line, 0, line_length);
        if (err == -1) {
            printf("Send to client headers end with ERROR\n");
            free(line);

            return NULL;
        }
        if (line[0] == '\r' && line[1] == '\n') {
            printf("get the end of the HTTP response header\n");
            free(line);
            break;
        }

        free(line);
    }

    printf("Start to send BODY\n");
    while (1) {
        ssize_t body_length;
        char *body = http_read_body(website_socket, &body_length, MAX_BUFFER_SIZE);
        if (body == NULL) {
            break; // Break the loop if the end of the request body is reached
        }

        int err = send_to_client(client_socket, body, 0, body_length);
        if (err == -1) {
            printf("Send to client body ended with ERROR\n");
            free(body);

            return NULL;
        }
        free(body);
    }

    printf("Send to client body was success\n");

    close(website_socket);
    close(client_socket);
    return NULL;
}

void *start_proxy_server(void *arg) {
    int server_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    init_server_socket(&server_socket, PORT, LISTENQ);

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        printf("Accepted a new connection\n");
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Create thread arguments
        threadArgs *args = malloc(sizeof(threadArgs));
        args->client_socket = client_socket;

        // Create a new thread for each client
        pthread_t thread;
        if (pthread_create(&thread, NULL, client_handler, args) != 0) {
            perror("Error creating thread");
            free(args);
            close(client_socket);
        } else {
            // Detach the thread to avoid memory leaks
            pthread_detach(thread);
        }
    }

    close(server_socket);
    return NULL;
}

int main() {
    pthread_t server_thread;

    // Create a thread for the proxy server
    if (pthread_create(&server_thread, NULL, start_proxy_server, NULL) != 0) {
        perror("Error creating server thread");
        exit(EXIT_FAILURE);
    }

    // Wait for the server thread to finish (this should never happen)
    pthread_join(server_thread, NULL);

    return 0;
}