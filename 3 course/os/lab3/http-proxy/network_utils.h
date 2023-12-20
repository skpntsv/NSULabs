#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <sys/types.h>
#include "proxy.h"

int create_client_socket(char *hostname, int port);
ssize_t send_request(int fd, void *buffer, size_t n);
ssize_t receive_request(int fd, void *buffer, size_t n);
void init_server_socket(int *server_socket, int port, int max_listeners);

http_request *http_read_header(int socket);
int http_connect(http_request *req);
char *http_read_body(int sockfd, ssize_t *length, int max_buffer);

#endif
