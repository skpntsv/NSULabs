#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <time.h>

#include "network_utils.h"
#include "http_message.h"

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
        close(*server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(*server_socket, max_listeners) < 0) {
        perror("Error listening");
        close(*server_socket);
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

char *http_read_body(int socket, ssize_t *length, int max_buffer) {
    if (length == NULL) {
        printf("ERROR: The length pointer supplied to http_read_chunk is NULL\n");
        return NULL;
    }

    if(socket == -1) {
        printf("ERROR: The socket given to http_read_chunk is invalid\n");
        return NULL;
    }

	char *buf = malloc(max_buffer);
    if (buf == NULL) {
        perror("malloc in http_read_body");
        return NULL;
    }

	memset(buf, '\0', max_buffer);

    time_t timeout = 5; 
    time_t start = time(NULL);

	ssize_t total_bytes = 0;
    ssize_t num_bytes = 0;

	while (total_bytes < max_buffer) {
        // if(time(NULL) - start > timeout) {
        //     printf("Request timed out\n");
        //     break; 
        // }

		num_bytes = recv(socket, buf + total_bytes, max_buffer - total_bytes, 0);
        //printf("%d\n", num_bytes);
        if (num_bytes == -1) {
            perror("recv in http_read_body");
            free(buf);

            return NULL;
        }
        if (num_bytes == 0) {
            break;
        }

        total_bytes += num_bytes;
	}

    if (total_bytes == 0) {
        printf("The end get body from website\n");

        return NULL;
    }

	//printf("Received: %d\n", (int)total_bytes);

	*length = total_bytes; 

	return buf; 
}

int http_connect(http_request *req) {
	char *host = (char*)list_get_key(&req->metadata_head, "Host"); 
    char *port = strstr(host, ":");

    if(port == NULL) {
        port = (char*)calloc(3, sizeof(char) + 1);
        strcat(port, "80");

        printf("Use default port 80 to connect the server\n");
    }
    else {
        host = strtok(host, ":");
        port++;

        printf("Use port %s to connect the server\n", port);
    }

    printf("Connecting to HTTP server: %s\n", host);

	if(host == NULL) {
		printf("no host:(");
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
    server_address.sin_port = htons(atoi(port));

    if (connect(website_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error connecting to server");
        close(website_socket);
        return -1;
    }

    printf("Successfully connected to HTTP server: %s\n", host);

	return website_socket;
}