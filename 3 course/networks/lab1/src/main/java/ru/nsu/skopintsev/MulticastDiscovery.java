package ru.nsu.skopintsev;

import org.jetbrains.annotations.NotNull;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;

public class MulticastDiscovery {
    private final String multicastGroupAddress;
    private final int multicastPort;
    private final MudakTable mudakTable;
    private DatagramSocket senderSocket;

    public MulticastDiscovery(String multicastGroupAddress, int multicastPort) {
        this.multicastGroupAddress = multicastGroupAddress;
        this.multicastPort = multicastPort;

        this.mudakTable = new MudakTable();

        try {
            this.senderSocket = new DatagramSocket(); // Инициализация сокса для отправки
        } catch (IOException e) {
            e.printStackTrace();
        }

        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("Sending LEAVE command before exiting...");
            sendCommand(MessageType.LEAVE);
        }));
    }

    public void start() {
            try (MulticastSocket receiveSocket = new MulticastSocket(multicastPort)) {
            InetAddress group = InetAddress.getByName(multicastGroupAddress);
            receiveSocket.joinGroup(group);

            Thread senderThread = new Thread(this::sendMulticast);
            Thread receiverThread = new Thread(this::receiveMulticast);

            senderThread.start();
            receiverThread.start();

            senderThread.join();
            receiverThread.join();
                
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void sendMulticast() {
        while (true) {
            sendCommand(MessageType.REPORT);
            try {
                Thread.sleep(10000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }

    public void sendCommand(@NotNull MessageType commandType) {
        try {
            InetAddress group = InetAddress.getByName(multicastGroupAddress);
            byte[] message = { (byte) commandType.getValue() };
            DatagramPacket packet = new DatagramPacket(message, message.length, group, multicastPort);
            senderSocket.send(packet);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void receiveMulticast() {
        try (MulticastSocket receiveSocket = new MulticastSocket(multicastPort)) {
            InetAddress group = InetAddress.getByName(multicastGroupAddress);
            receiveSocket.joinGroup(group);

            byte[] buffer = new byte[1];
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

            while (true) {
                receiveSocket.receive(packet);
                byte receivedMessageType = packet.getData()[0];
                String user = packet.getAddress().getHostAddress() + ":" + packet.getPort();
                MessageType messageType = MessageType.fromValue(receivedMessageType);

                if (messageType != null) {
                    switch (messageType) {
                        case REPORT -> {
                            System.out.println("Received MUDAK Report message from " + user);
                            mudakTable.updateEntry(user);
                        }
                        case LEAVE -> {
                            System.err.println("Received MUDAK Leave message from " + user);
                            mudakTable.removeEntry(user);
                        }
                        default -> System.out.println("Received unknown message from " + user);
                    }
                }
                mudakTable.killZombie();
                mudakTable.printTable();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
