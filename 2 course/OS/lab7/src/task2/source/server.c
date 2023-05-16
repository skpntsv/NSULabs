#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

void handle_client(int client_socket) {
    char buffer[1024];
    ssize_t bytes_read;

    // Чтение данных от клиента
    bytes_read = read(client_socket, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        perror("Ошибка чтения данных от клиента");
        close(client_socket);
        exit(1);
    }

    // Отправка данных обратно клиенту
    if (write(client_socket, buffer, bytes_read) != bytes_read) {
        perror("Ошибка отправки данных клиенту");
        close(client_socket);
        exit(1);
    }

    close(client_socket);
    exit(0);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Создание TCP-сокета
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Ошибка создания сокета");
        exit(1);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Привязка сокета к заданному ip и порту
    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("Ошибка привязки сокета");
        close(server_socket);
        exit(1);
    }

    // Прослушивание входящих соединений
    if (listen(server_socket, 10) == -1) {
        perror("Ошибка прослушивания сокета");
        close(server_socket);
        exit(1);
    }

    printf("Сервер запущен и ожидает подключений...\n");

    while (1) {
        // Принятие входящего соединения
        client_socket = accept(server_socket, (struct sockaddr*) &client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Ошибка при принятии соединения");
            close(server_socket);
            exit(1);
        }

        // Создание нового процесса для обработки клиента
        pid_t pid = fork();
        if (pid == -1) {
            perror("Ошибка создания процесса");
            close(client_socket);
            close(server_socket);
            exit(1);
        } else if (pid == 0) {
            // Мы находимся в дочернем процессе
            close(server_socket);
            handle_client(client_socket);
        } else {
            // Мы находимся в родительском процессе
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}
