// network_utils.h
#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <sys/types.h>

int create_socket(char *hostname, int port);
ssize_t send_request(int fd, void *buffer, size_t n);
ssize_t receive_request(int fd, void *buffer, size_t n);
void init_server_socket(int *server_socket, int port, int max_listeners);

#endif
