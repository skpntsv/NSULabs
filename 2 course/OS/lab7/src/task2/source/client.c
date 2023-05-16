#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Создание TCP-сокета
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Ошибка создания сокета");
        exit(1);
    }

    // Настройка адреса сервера
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT);

    // Подключение к серверу
    if (connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) {
        perror("Ошибка подключения к серверу");
        close(clientSocket);
        exit(1);
    }
    printf("Подключени к серверу выполнено успешно.\n");

    serverAddr.sin_port = htons(getpid());

    // Отправка данных на сервер
    while (1) {
        printf("Введите сообщение для отправки серверу (или 'q' для выхода): ");
        fgets(buffer, sizeof(buffer), stdin);

        // Удаление символа новой строки ('\n') из введенной строки
        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        // Выход из цикла, если введена команда "q"
        if (strcmp(buffer, "q") == 0) {
            break;
        }

        // Отправка данных на сервер
        if (write(clientSocket, buffer, len) == -1) {
            perror("Ошибка отправки данных на сервер");
            close(clientSocket);
            exit(1);
        }

        // Чтение данных от сервера
        bytes_read = read(clientSocket, buffer, len);
        if (bytes_read == -1) {
            perror("Ошибка чтения данных от сервера");
            close(clientSocket);
            exit(1);
        }

        // Вывод полученных данных
        printf("Получено от сервера: %s\n", buffer);
    }

    close(clientSocket);
    return 0;
}
