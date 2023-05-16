#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

struct Client {
    int socket;
    struct sockaddr_in address;
};

void handle_client(struct Client* client) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("Новое подключение от клиента: %s:%d\n",
        inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));

    while (1) {
        // Чтение данных от клиента
        bytes_read = read(client->socket, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            printf("Клиент отключился: %s:%d\n",
                inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));
            close(client->socket);
            exit(0);
        }

        // Отправка данных обратно клиенту
        if (write(client->socket, buffer, bytes_read) != bytes_read) {
            perror("Ошибка отправки данных клиенту");
            close(client->socket);
            exit(1);
        }
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Создание TCP-сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Ошибка создания сокета");
        exit(1);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // Привязка сокета к заданному ip и порту
    if (bind(serverSocket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("Ошибка привязки сокета");
        close(serverSocket);
        exit(1);
    }

    // Прослушивание входящих соединений
    if (listen(serverSocket, 10) == -1) {
        perror("Ошибка прослушивания сокета");
        close(serverSocket);
        exit(1);
    }

    printf("Сервер запущен и ожидает подключений...\n");

    while (1) {
        // Принятие входящего соединения
        clientSocket = accept(serverSocket, (struct sockaddr*) &client_addr, &addr_len);
        if (clientSocket == -1) {
            perror("Ошибка при принятии соединения");
            close(serverSocket);
            exit(1);
        }

        // Создание нового процесса для обработки клиента
        pid_t pid = fork();
        if (pid == -1) {
            perror("Ошибка создания процесса");
            close(clientSocket);
            close(serverSocket);
            exit(1);
        } else if (pid == 0) {
            // Мы находимся в дочернем процессе
            close(serverSocket);
            struct Client client;
            client.socket = clientSocket;
            client.address = client_addr;
            handle_client(&client);
        } else {
            // Мы находимся в родительском процессе
            close(clientSocket);
            if (waitpid(pid, NULL, WNOHANG) == -1) {
                perror("waitpid");
                exit(1);
            }
        }
    }
    close(serverSocket);
    return 0;
}
