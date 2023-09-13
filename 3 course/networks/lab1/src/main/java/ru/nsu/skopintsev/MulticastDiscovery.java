package ru.nsu.skopintsev;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;

public class MulticastDiscovery {
    private final String multicastGroupAddress;
    private final int multicastPort;

    public MulticastDiscovery(String multicastGroupAddress, int multicastPort) {
        this.multicastGroupAddress = multicastGroupAddress;
        this.multicastPort = multicastPort;
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
        try (DatagramSocket senderSocket = new DatagramSocket()) {
            InetAddress group = InetAddress.getByName(multicastGroupAddress);

            while (true) {
                byte[] message = { 1 };
                DatagramPacket packet = new DatagramPacket(message, message.length, group, multicastPort);
                senderSocket.send(packet);
                Thread.sleep(10000);
            }
        } catch (IOException | InterruptedException e) {
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
                switch (receivedMessageType) {
                    case 1 -> System.out.println("Received MUDAK Report message from " + packet.getAddress());
                    case 2 -> System.out.println("Received MUDAK Leave message from " + packet.getAddress());
                    default -> System.out.println("Received unknown message from " + packet.getAddress());
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
