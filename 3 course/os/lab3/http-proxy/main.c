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
#include "proxy.h"

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

    struct Cache_t *next;
} cache;

typedef struct ThreadArgs_t {
    int client_socket;
    // TODO добавить кэш
} threadArgs;


char *read_line(int sockfd)
{
    int buffer_size = 2;
    char *line = (char*)malloc(sizeof(char)*buffer_size+1);
    char c;
    int length = 0;
    int counter = 0;

    while(1) {
        length = recv(sockfd, &c, 1, 0);
        line[counter++] = c;

        if(c == '\n') {
            line[counter] = '\0';
            return line;
        }

        if(counter == buffer_size) {
            buffer_size *= 2;
            line = (char*)realloc(line, sizeof(char)*buffer_size);
        }
    }

    return NULL;
}

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

    printf("CONTENT PATH: %s\n", content->path);
}

int http_request_send(int sockfd, http_request *request) {
    char *request_buffer = http_build_request(request);

    if(send(sockfd, request_buffer, strlen(request_buffer), 0) == -1) {
        free(request_buffer);
        perror("send");
        return 1;
    }
    free(request_buffer);

    printf("Sent HTTP header to web server\n");

    return 0;
}

int send_to_client(int client_sockfd, char data[], int packages_size, ssize_t length) {
    if(packages_size < 1) {
        if(send(client_sockfd, data, length, 0) == -1)
        {
            perror("Couldn't send data to the client.");
            return -1;
        }
    } else {
        int p;
        for(p = 0; p*packages_size + packages_size < length; p++) {
            if(send(client_sockfd, (data + p*packages_size), packages_size, 0) == -1)
            {
                perror("Couldn't send any or just some data to the client. (loop)\n");
                return -1;
            }
        }

        if (p*packages_size < length) {
            if(send(client_sockfd, (data + p*packages_size), length - p*packages_size, 0) == -1) {
                perror("Couldn't send any or just some data to the client.\n");
                return -1;
            }
        }
    }

    return 0;
}

void* client_handler(void* args) {
    threadArgs *thread_args = (threadArgs*)args;
    int client_socket = thread_args->client_socket;

    char *line;
    int website_socket;

    http_request *request = http_read_header(client_socket);
    if (request == NULL) {
        printf("Failed to parse the header\n");
        
        return NULL;
    }

    website_socket = http_connect(request);
    if (website_socket == -1) {
        printf("Failed to connect to host\n");

        http_request_destroy(request);
        return NULL;
    }

    http_request_send(website_socket, request);
    http_request_destroy(request);
    
    printf("Start to retrieve the response header\n");
    int line_lenght;
    while (1) {
        line = read_line(website_socket);
        line_lenght = strlen(line);
        send_to_client(client_socket, line, 0, line_lenght);

        if (line[0] == '\r' && line[1] == '\n') {
            printf("get the end of the HTTP response header\n");
            free(line);
            break;
        }

        free(line);
    }



    ////////////////////////
    // char buffer[MAX_BUFFER_SIZE];

    // int bytes_read = receive_request(client_socket, buffer, sizeof(buffer));
    // if (bytes_read < 0) {
    //     perror("receive_request");
    //     close(client_socket);

    //     return NULL;
    // }
    // //printf("buffer =\n %s\n", buffer);

    // char method[100], url[MAX_URL_SIZE], version[MAX_BUFFER_SIZE];
    // sscanf(buffer, "%s %s %s ", method, url, version);
    // printf("Get request: %s %s %s\n", buffer, url, version);

    // if (strcasecmp(method, "GET")) {
    //     printf("This proxy doesn't support this method - %s\n", method);
    //     close(client_socket);

    //     return NULL;
    // }

    // request content;
    // parse_url(url, &content);
    
    // // Создаём новый HTTP запрос
    // char new_request[MAX_BUFFER_SIZE];
    // sprintf(new_request, "GET %s HTTP/1.0\nHost: %s\n", content.path, content.host);

    // int website_socket = create_client_socket(content.host, content.port);

    // // отправить тот же самый реквест что и пришел нам
    // send_request(website_socket, buffer, sizeof(buffer));
    // printf("Send request to website:\n %s\n", buffer);

    // // отправить самостоятельно созданный реквест
    // // send_request(website_socket, buffer, sizeof(buffer));
    // // printf("Send request to website:\n %s\n", buffer);

    // char response[MAX_BUFFER_SIZE];
    // receive_request(website_socket, response, sizeof(response));
    // printf("Get request from website:\n%s\n", response);

    // close(website_socket);

    // size_t response_length = strlen(response);
    // send_request(client_socket, response, response_length);
    // //send(client_socket, response, sizeof(response), NULL);
    // printf("Send request to client:\n%s\n", response);

    close(client_socket);
    return NULL;
}

void start_proxy_server() {
    threadArgs args;
    int server_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    init_server_socket(&server_socket, PORT, LISTENQ);

    while (1) {
        args.client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        printf("Принято новое подключение\n");
        if (args.client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // pthread_t thread;
        // pthread_create(&thread, NULL, client_handler, &args);
        // pthread_detach(thread);
        client_handler(&args);
    }

    close(server_socket);
}

int main() {
    start_proxy_server();

    return 0;
}