#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>

#include "network_utils.h"

int create_client_socket(char *hostname, int port) {
    int client_socket;
    struct hostent *host_info;
    struct sockaddr_in server_address;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        return -1;
    }

    if ((host_info = gethostbyname(hostname)) == NULL) {
        perror("Error getting host by name");
        close(client_socket);
        return -1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    memcpy(&server_address.sin_addr.s_addr, host_info->h_addr_list[0], host_info->h_length);
    server_address.sin_port = htons(port);

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error connecting to server");
        close(client_socket);
        return -1;
    }

    return client_socket;
}

ssize_t send_request(int fd, void *buffer, size_t n) {
    size_t remaining = n;
    ssize_t sent;
    char *bufp = buffer;

    while (remaining > 0) {
        if ((sent = write(fd, bufp, remaining)) <= 0) {
            if (errno == EINTR) {
                sent = 0;
            } else {
                perror("Error writing to socket");
                return -1;
            }
        }

        remaining -= sent;
        bufp += sent;
        
    }

    return n;
}

ssize_t receive_request(int fd, void *buffer, size_t n) {
    size_t remaining = n;
    ssize_t received;
    char *bufp = buffer;

    while (remaining > 0) {
        if ((received = read(fd, bufp, remaining)) < 0) {
            if (errno == EINTR) {
                received = 0;
            } else {
                perror("Error reading from socket");
                return -1;
            }
        } else if (received == 0) {
            break; // EOF - конец строки
        }

        remaining -= received;
        bufp += received;
    }

    return n - remaining;
}

void init_server_socket(int *server_socket, int port, int max_listeners) {
    *server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(*server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(*server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(*server_socket, max_listeners) < 0) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Proxy server started and listening on port %d\n", port);
}