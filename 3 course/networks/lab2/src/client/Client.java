package client;

import java.io.File;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import utils.FileTransferProtocol;

public class Client {
    private final String HOST;
    private final int PORT;
    private final String filePath;

    public Client(String host, int port, String filePath) {
        this.HOST = host;
        this.PORT = port;
        this.filePath = filePath;

        start();
    }

    public void start() {
        try (Socket socket = new Socket(HOST, PORT)) {
            FileTransferProtocol protocol = new FileTransferProtocol(socket);

            File fileToSend = new File(filePath);
            if (fileToSend.exists()) {
                protocol.sendFile(fileToSend);
            } else {
                System.err.println("Файл не найден: " + filePath);
            }
        } catch (UnknownHostException e) {
            System.err.println(HOST + ":" + PORT + " unknown");
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
