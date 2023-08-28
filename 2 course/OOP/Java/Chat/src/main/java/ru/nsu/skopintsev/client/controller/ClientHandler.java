package ru.nsu.skopintsev.client.controller;

import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.client.view.ClientUI;
import ru.nsu.skopintsev.client.xmlUtils.XMLMessageParser;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.Set;

public class ClientHandler implements Runnable {
    private final Selector selector;
    private final ClientUI clientUI;
    private boolean isRunning = true;

    public ClientHandler(@NotNull Selector selector, ClientUI clientUI) {
        this.selector = selector;
        this.clientUI = clientUI;
    }

    @Override
    public void run() {
        try {
            while (isRunning) {
                int readyChannels = selector.select();

                if (readyChannels > 0) {
                    Set<SelectionKey> selectionKeys = selector.selectedKeys();
                    for (SelectionKey selectionKey : selectionKeys) {
                        if (selectionKey.isReadable()) {
                            readHandler(selectionKey);
                        }
                    }
                    selectionKeys.clear();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void readHandler(@NotNull SelectionKey selectionKey) {
        SocketChannel socketChannel = (SocketChannel) selectionKey.channel();
        ByteBuffer sizeBuffer = ByteBuffer.allocate(4);
        int bytesRead = 0;
        while (sizeBuffer.hasRemaining()) {
            try {
                bytesRead = socketChannel.read(sizeBuffer);
            } catch (IOException e) {
                disconnectClient(socketChannel);
                return;
            }
        }

        if (bytesRead == -1) {
            disconnectClient(socketChannel);
            return;
        }
        if (sizeBuffer.position() == 4) {
            sizeBuffer.flip();
            int messageSize = sizeBuffer.getInt();

            ByteBuffer messageBuffer = ByteBuffer.allocate(messageSize);

            while (messageBuffer.hasRemaining()) {
                try {
                    socketChannel.read(messageBuffer);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
            messageBuffer.flip();

            messageHandler(messageBuffer);
        }
    }

    private void messageHandler(ByteBuffer messageBuffer) {
        XMLMessageParser xmlParser = new XMLMessageParser(messageBuffer);

        switch (xmlParser.extractMessageType()) {
            case message -> clientUI.displayMessage(xmlParser.extractUsername() + ": " + xmlParser.extractMessageContent());
            case userlogin -> clientUI.displayMessage("User " + xmlParser.extractUsername() + " has joined to the chat");
            case userlogout -> clientUI.displayMessage("User " + xmlParser.extractUsername() + " has left the chat");
            case listusers -> clientUI.updateUserList(xmlParser.extractListOfUsers());
            case error -> {
                clientUI.displayMessage(xmlParser.extractUsername() + ": " + xmlParser.extractMessageContent());
                clientUI.restartReg();
            }
            case success -> {
                clientUI.displayMessage("Welcome to the chat!");
                clientUI.startTimer();
            }
        }
    }

    private void disconnectClient(@NotNull SocketChannel socketChannel) {
        isRunning = false;
        try {
            socketChannel.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        socketChannel.keyFor(selector).cancel();
        System.err.println("We was closed by server");
        clientUI.displayMessage("The server is down");
        clientUI.displayMessage("¯\\_(ツ)_/¯");
        clientUI.displayMessage("Try again later");

        clientUI.restartReg();
    }
}