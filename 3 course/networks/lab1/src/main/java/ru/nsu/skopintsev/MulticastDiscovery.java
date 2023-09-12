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
                String message = "Hello, I'm here!";
                DatagramPacket packet = new DatagramPacket(message.getBytes(), message.length(), group, multicastPort);
                senderSocket.send(packet);
                Thread.sleep(2000);
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void receiveMulticast() {
        try (MulticastSocket receiveSocket = new MulticastSocket(multicastPort)) {
            InetAddress group = InetAddress.getByName(multicastGroupAddress);
            receiveSocket.joinGroup(group);

            byte[] buffer = new byte[1024];
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

            while (true) {
                receiveSocket.receive(packet);
                String receivedMessage = new String(packet.getData(), 0, packet.getLength());
                System.out.println("Received message from " + packet.getAddress() + ": " + receivedMessage);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
