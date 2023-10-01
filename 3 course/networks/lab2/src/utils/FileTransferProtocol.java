package utils;

import java.io.*;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

public class FileTransferProtocol {
    private final DataInputStream dataInputStream;
    private final DataOutputStream dataOutputStream;

    public FileTransferProtocol(Socket socket) throws IOException {
        dataInputStream = new DataInputStream(socket.getInputStream());
        dataOutputStream = new DataOutputStream(socket.getOutputStream());
    }

    public void sendFile(File file) throws IOException {
        String fileName = file.getName();
        long fileSize = file.length();

        dataOutputStream.writeShort(fileName.length());

        dataOutputStream.writeUTF(fileName);

        dataOutputStream.writeLong(fileSize);

        try (FileInputStream fileInputStream = new FileInputStream(file)) {
            byte[] buffer = new byte[1024];
            int bytesRead;

            while ((bytesRead = fileInputStream.read(buffer)) != -1) {
                dataOutputStream.write(buffer, 0, bytesRead);
            }
            dataOutputStream.flush();
        }
    }

    public void receiveFile(String savePath) throws IOException {
        int titleLength = dataInputStream.readShort();

        byte[] fileNameBytes = new byte[titleLength];
        dataInputStream.readFully(fileNameBytes);
        String fileName = new String(fileNameBytes, StandardCharsets.UTF_8);

        long fileSize = dataInputStream.readLong();

        File outputFile = new File(savePath, fileName);
        try (FileOutputStream fileOutputStream = new FileOutputStream(outputFile)) {
            byte[] buffer = new byte[1024];
            int bytesRead;
            long totalBytesReceived = 0;

            while (totalBytesReceived < fileSize && (bytesRead = dataInputStream.read(buffer)) != -1) {
                fileOutputStream.write(buffer, 0, bytesRead);
                totalBytesReceived += bytesRead;
            }
            fileOutputStream.flush();
        }
    }

}
