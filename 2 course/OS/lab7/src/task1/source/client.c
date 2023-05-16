#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    char buffer[MAX_BUFFER_SIZE];

    // Создание UDP сокета
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT);

    while (1) {
        printf("Введите сообщение для отправки серверу (или 'q' для выхода): ");
        fgets(buffer, sizeof(buffer), stdin);

        // Удаление символа новой строки ('\n') из введенной строки
        size_t length = strlen(buffer);
        if (buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
        }
        // Отправка данных на сервер
        ssize_t sentBytes = sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr*) &serverAddr, addrLen);
        if (sentBytes == -1) {
            perror("Ошибка при отправке данных на сервер");
            exit(EXIT_FAILURE);
        }

        // Выход из цикла, если введена команда "q"
        if (strcmp(buffer, "q") == 0) {
            break;
        }

        // Получение данных от сервера
        ssize_t receivedBytes = recvfrom(clientSocket, buffer, sizeof(buffer), 0, NULL, NULL);
        if (receivedBytes == -1) {
            perror("Ошибка при получении данных от сервера");
            exit(EXIT_FAILURE);
        }

        // Вывод полученных данных
        printf("Получено от сервера: %s\n", buffer);
    }

    // Закрытие сокета
    pclose(clientSocket);

    return 0;
}
