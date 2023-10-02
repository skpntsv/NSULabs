package server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Server {
    private final int PORT;

    public Server(int port) {
        this.PORT = port;

        String savePath = "./uploads";
        Path directoryPath = Paths.get(savePath);
        if (!Files.exists(directoryPath)) {
            try {
                Files.createDirectories(directoryPath);
            } catch (IOException e) {
                System.err.println("Не удалось создать директорию" + e.getMessage());
            }
        }

        start();
    }

    public void start() {
        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("Сервер запущен и ожидает подключения: " + serverSocket.getInetAddress() + ": " + serverSocket.getLocalPort());

            while (!serverSocket.isClosed()) {
                try {
                    Socket clientSocket = serverSocket.accept();
                    System.out.println("Подключился новый клиент: " + clientSocket.getInetAddress().getHostAddress() + ": " + clientSocket.getPort());

                    ClientHandler clientHandler = new ClientHandler(clientSocket);
                    Thread clientThread = new Thread(clientHandler);
                    clientThread.start();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        } catch (IOException e) {
            System.out.println("Не удалось создать сокет " + e.getMessage());
        }
    }
}
