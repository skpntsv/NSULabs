package ru.nsu.skopintsev;

import org.jetbrains.annotations.NotNull;

import java.io.IOException;
import java.net.*;
import java.util.Timer;
import java.util.TimerTask;

public class MulticastDiscovery {
    private final String multicastGroupAddress;
    private final int multicastPort;
    private final MudakTable mudakTable;
    private final Timer timer = new Timer();
    private DatagramSocket senderSocket;

    public MulticastDiscovery(String multicastGroupAddress, int multicastPort) {
        this.multicastGroupAddress = multicastGroupAddress;
        this.multicastPort = multicastPort;

        this.mudakTable = new MudakTable();

        try {
            this.senderSocket = new DatagramSocket(); // Инициализация сокса для отправки
        } catch (SocketException e) {
            System.err.println("Create datagram socker failed " + e.getMessage());
        }

        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("Sending LEAVE command before exiting...");
            sendCommand(MessageType.LEAVE);
        }));

        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                mudakTable.printTable();
            }
        }, 1000, 900);
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
        try {
            while (true) {
                sendCommand(MessageType.REPORT);
                Thread.sleep(10000);
            }
        } catch (InterruptedException e) {
            System.err.println("sendMulticast interrupted: " + e.getMessage());
        } catch (Exception e) {
            System.err.println("sendMulticast error: " + e.getMessage());
        }
    }

    public void sendCommand(@NotNull MessageType commandType) {
        try {
            InetAddress group = InetAddress.getByName(multicastGroupAddress);
            byte[] message = { (byte) commandType.getValue() };
            DatagramPacket packet = new DatagramPacket(message, message.length, group, multicastPort);
            senderSocket.send(packet);
        } catch (IOException e) {
            System.err.println("sendCommand error: " + e.getMessage());
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
            }
        } catch (IOException e) {
            System.err.println("receiveMulticast error: " + e.getMessage());
        }
    }
}
