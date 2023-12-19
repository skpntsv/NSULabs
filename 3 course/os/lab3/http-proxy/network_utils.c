#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>

#include "network_utils.h"
#include "http_message.h"

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
    ssize_t received = 1;
    char *bufp = buffer;

    while (remaining > 0) {
        printf("Прочитано %d байт\n", received);
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
    printf("Я всё прочитал!");
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
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(*server_socket, max_listeners) < 0) {
        perror("Error listening");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Proxy server started and listening on port %d\n", port);
}


http_request *http_read_header(int socket) {
	http_request *request;
	http_request_init(&request); 

	char *line; 
	line = read_line(socket); 
	http_parse_method(request, line); 

	while(1) {
		line = read_line(socket); 
		if(line[0] == '\r' && line[1] == '\n') {
			break; 
		}

		http_parse_metadata(request, line); 

		free(line); 
	}

	return request;
}

int http_connect(http_request *req) {
	char *host = (char*)list_get_key(&req->metadata_head, "Host"); 
    char *port = strstr(host, ":");

    if(port == NULL) {
        port = calloc(3, sizeof(char));
        strncat(port, "80", 2);

        printf("Использую дефолтный порт 80 для подключения к серверу");
    }
    else {
        host = strtok(host, ":");
        port++;

        printf("Использую порт %s для подключения к серверу", port);
    }

    printf("Connecting to HTTP server: %s\n", host);

	if(host == NULL) {
		printf("Имя хоста потерялось");
		return -1; 
	}
    
    int website_socket;
    struct hostent *host_info;
    struct sockaddr_in server_address;

    if ((website_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        return -1;
    }

    if ((host_info = gethostbyname(host)) == NULL) {
        perror("Error getting host by name");
        close(website_socket);
        return -1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    memcpy(&server_address.sin_addr.s_addr, host_info->h_addr_list[0], host_info->h_length);
    server_address.sin_port = htons(port);

    if (connect(website_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error connecting to server");
        close(website_socket);
        return -1;
    }

	return website_socket;
}