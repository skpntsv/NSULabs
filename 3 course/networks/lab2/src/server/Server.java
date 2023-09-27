package server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {
    private final int PORT = 1234;

    public Server() {
        start();
    }

    public void start() {
        try (ServerSocket server = new ServerSocket(PORT)) {
            System.out.println("Сервер запущен и ожидает подключения к порту:" + PORT);

            while (!server.isClosed()) {
                Socket client = server.accept();
                System.out.println("Connection accepted: " + client.getInetAddress() + ":" + client.getPort());

                ClientHandler clientHandler = new ClientHandler(client);
                Thread clientThread = new Thread(clientHandler);
                clientThread.start();
            }

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
