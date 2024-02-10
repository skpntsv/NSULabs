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
    ssize_t line_lenght;
	line = read_line(socket, &line_lenght);
    printf("Header: %s\n", line);
	http_parse_method(request, line); 

	while(1) {
		line = read_line(socket, &line_lenght); 
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

    char *buf = (char*)malloc(max_buffer + 1);
    if (buf == NULL) {
        perror("malloc in http_read_body");
        return NULL;
    }

	memset(buf, '\0', max_buffer + 1);

	ssize_t total_bytes = 0;
    ssize_t num_bytes = 0;

	while (total_bytes < max_buffer) {

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

int send_to_client(int client_socket, char* data, int packages_size, ssize_t length) {
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
        perror("Couldn't send data to the client");
        return -1;
    }

    return 0;
}

char *read_line(int socket, ssize_t *lenght) {
    int buffer_size = 3;
    char *line = (char*)malloc(sizeof(char) * buffer_size + 1);
    if (!line) {
        perror("malloc in read_line()");
        abort();
    }
    char c;
    ssize_t recbytes = 0;
    int counter = 0;

    while(1) {
        recbytes = recv(socket, &c, 1, 0);
        if (recbytes == -1) {
            perror("recv");
        } else if (recbytes == 0) {
            line[0] = '\r';
            line[1] = '\n';
            line[2] = '\0';

            *lenght = 3;
            return line;
        }
        line[counter++] = c;
        //printf("%d\n", line[counter]);

        if (c == '\n') {
            line[counter] = '\0';

            *lenght = counter;
            return line;
        }

        if (counter >= buffer_size) {
            buffer_size += 1;
            //printf("line: %s |strlen: %d | buffer_size: %d\n", line, strlen(line), buffer_size);
            char *temp = (char*)realloc(line, sizeof(char) * buffer_size + 1);
            if (!temp) {
                perror("realloc in read_line()");
                free(line);
                abort();
            }
            line = temp;
        }
    }

    return NULL;
}