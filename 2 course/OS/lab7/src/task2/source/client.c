#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];
    ssize_t bytes_read;

    // Создание TCP-сокета
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Ошибка создания сокета");
        exit(1);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Замените на нужный ip сервера
    server_addr.sin_port = htons(SERVER_PORT);

    // Подключение к серверу
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Ошибка подключения к серверу");
        close(client_socket);
        exit(1);
    }

    // Отправка данных на сервер
    while (1) {
        //strcpy(buffer, "Привет, сервер!");
        fgets(buffer, sizeof(buffer), stdin);
        if (strcmp(buffer, "q") == 0) {
            break;
        }
        if (write(client_socket, buffer, strlen(buffer)) == -1) {
            perror("Ошибка отправки данных на сервер");
            close(client_socket);
            exit(1);
        }

        // Чтение данных от сервера
        bytes_read = read(client_socket, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("Ошибка чтения данных от сервера");
            close(client_socket);
            exit(1);
    }

    printf("Полученные данные от сервера: %.*s\n", (int)bytes_read, buffer);
    }

    close(client_socket);
    return 0;
}
