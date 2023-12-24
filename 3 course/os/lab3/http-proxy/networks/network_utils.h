#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <sys/types.h>
#include "../proxy.h"

void init_server_socket(int *server_socket, int port, int max_listeners);

http_request *http_read_header(int socket);
int http_connect(http_request *req);
unsigned char *http_read_body(int sockfd, ssize_t *length, int max_buffer);
int send_to_client(int client_socket, char data[], int packages_size, ssize_t length);
unsigned char *read_line(int socket);

#endif
