package server;

import utils.FileTransferProtocol;

import java.io.IOException;
import java.net.Socket;

public class ClientHandler implements Runnable {
    private final Socket clientSocket;

    public ClientHandler(Socket clientSocket) {
        this.clientSocket = clientSocket;
    }

    @Override
    public void run() {
        try {
            FileTransferProtocol protocol = new FileTransferProtocol(clientSocket);
            protocol.receiveFile("./uploads");

            clientSocket.close();
            //System.out.println("Клиент отключился: " + clientSocket.getInetAddress().getHostAddress() + ":" + clientSocket.getPort());
        } catch (IOException e) {
            System.err.println("Ошибка при обработке клиента: " + e.getMessage());
        } finally {
            try {
                if (!clientSocket.isClosed()) {
                    clientSocket.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println("Клиент отключился: " + clientSocket.getInetAddress().getHostAddress() + ": " + clientSocket.getPort());
        }
    }
}
