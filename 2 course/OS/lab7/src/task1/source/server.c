#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 8888

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    char buffer[MAX_BUFFER_SIZE];

    // Создание UDP сокета
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVER_PORT);

    // Привязка сокета к адресу сервера
    if (bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) {
        perror("Ошибка при привязке сокета");
        exit(EXIT_FAILURE);
    }

    printf("UDP сервер запущен и прослушивает порт %d\n", SERVER_PORT);

    while (1) {
        // Получение данных от клиента
        ssize_t receivedBytes = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*) &clientAddr, &addrLen);
        if (receivedBytes == -1) {
            perror("Ошибка при получении данных от клиента");
            exit(EXIT_FAILURE);
        }

        printf("Получены данные от клиента %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Отправка данных обратно клиенту
        ssize_t sentBytes = sendto(serverSocket, buffer, receivedBytes, 0, (struct sockaddr*) &clientAddr, addrLen);
        if (sentBytes == -1) {
            perror("Ошибка при отправке данных клиенту");
            exit(EXIT_FAILURE);
        }
    }

    // Закрытие сокета
    pclose(serverSocket);

    return 0;
}
