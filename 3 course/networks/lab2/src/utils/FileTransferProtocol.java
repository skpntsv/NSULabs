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
        dataOutputStream.write(fileName.getBytes(StandardCharsets.UTF_8));
        dataOutputStream.writeLong(fileSize);

        try (FileInputStream fileInputStream = new FileInputStream(file)) {
            byte[] buffer = new byte[8192];
            int bytesRead;

            while ((bytesRead = fileInputStream.read(buffer)) != -1) {
                dataOutputStream.write(buffer, 0, bytesRead);
            }
            dataOutputStream.flush();
        }

        boolean isSuccess = dataInputStream.readBoolean();
        if (isSuccess) {
            System.out.println("Прием файла завершился успешно.");
        } else {
            System.out.println("Прием файла завершился неудачно.");
        }
    }

    public void receiveFile(String savePath) throws IOException {
        int titleLength = dataInputStream.readShort();
        byte[] fileNameBytes = new byte[titleLength];
        int bytesRead = 0;

        while (bytesRead < titleLength) {
            int read = dataInputStream.read(fileNameBytes, bytesRead, titleLength - bytesRead);
            if (read == -1) {
                throw new IOException("Не удалось считать имя файла.");
            }
            bytesRead += read;
        }

        String fileName = new String(fileNameBytes, StandardCharsets.UTF_8);

        long fileSize = dataInputStream.readLong();

        File outputFile = new File(savePath, fileName);
        try (FileOutputStream fileOutputStream = new FileOutputStream(outputFile)) {
            byte[] buffer = new byte[8192];
            long totalBytesReceived = 0;
            long startTime = System.currentTimeMillis();

            while (totalBytesReceived < fileSize && (bytesRead = dataInputStream.read(buffer)) != -1) {
                fileOutputStream.write(buffer, 0, bytesRead);
                totalBytesReceived += bytesRead;

                long currentTime = System.currentTimeMillis();
                if (currentTime - startTime >= 3000 || totalBytesReceived == fileSize) {
                    long elapsedTime = currentTime - startTime;
                    double instantSpeed = (double) bytesRead / (double) elapsedTime * 1000; // байтов в секунду
                    double averageSpeed = (double) totalBytesReceived / (double) elapsedTime * 1000; // байтов в секунду

                    System.out.println("Мгновенная скорость приема: " + instantSpeed + " байт/сек");
                    System.out.println("Средняя скорость за сеанс: " + averageSpeed + " байт/сек");

                    startTime = currentTime;
                }
            }
            fileOutputStream.flush();

            if (totalBytesReceived == fileSize) {
                dataOutputStream.writeBoolean(true); // Успех
            } else {
                dataOutputStream.writeBoolean(false); // Неудача
            }
            dataOutputStream.flush();
        }
    }
}

