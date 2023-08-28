package ru.nsu.skopintsev.client.controller;

import org.jetbrains.annotations.NotNull;
import org.w3c.dom.Document;
import ru.nsu.skopintsev.utils.MessageType;
import ru.nsu.skopintsev.utils.XMLUtils;
import ru.nsu.skopintsev.client.view.ClientUI;
import ru.nsu.skopintsev.utils.ConfigParser;
import ru.nsu.skopintsev.client.xmlUtils.*;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.Timer;
import java.util.TimerTask;

public class ClientController {
    private final String hostname;
    private final int port;
    private SocketChannel socketChannel;
    private final ClientUI clientUI;

    public ClientController() {
        this.hostname = ConfigParser.HOSTNAME;
        this.port = ConfigParser.PORT;

        this.clientUI = new ClientUI(this);
        clientUI.start();

    }

    private void connect() {
        Selector selector;
        try {
            socketChannel = SocketChannel.open(new InetSocketAddress(hostname, port));
            selector = Selector.open();
            socketChannel.configureBlocking(false);
            socketChannel.register(selector, SelectionKey.OP_READ);
            sendRequest(MessageType.login);

        } catch (IOException e) {
            clientUI.displayMessage("Server was not found, try again");
            clientUI.restartReg();
            return;
        }
        new Thread(new ClientHandler(selector, clientUI)).start();
    }

    public void registration() {
        connect();
    }

    public void sendMessage(@NotNull String message) {
        XMLMessageBuilder messageBuilder = new XMLMessageBuilder(MessageType.message, clientUI.getNickname(), message);
        Document document = messageBuilder.getDoc();

        sendXMLToSocket(document);
    }

    public void sendRequest(MessageType type) {
        XMLMessageBuilder messageBuilder = new XMLMessageBuilder(type, clientUI.getNickname(), null);
        Document document = messageBuilder.getDoc();

        sendXMLToSocket(document);
    }

    private void sendXMLToSocket(@NotNull Document document) {
        ByteBuffer sizeBuffer = ByteBuffer.allocate(4);
        ByteBuffer messageBuffer = XMLUtils.xmlToByteBuffer(document);

        int messageSize = messageBuffer.remaining();
        sizeBuffer.putInt(messageSize);
        sizeBuffer.flip();

        try {
            while (sizeBuffer.hasRemaining()) {
                socketChannel.write(sizeBuffer);
            }

            while (messageBuffer.hasRemaining()) {
                socketChannel.write(messageBuffer);
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public void startTimer() {
        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (socketChannel != null) {
                    if (socketChannel.isOpen()) {
                        sendRequest(MessageType.list);
                    }
                }
            }
        }, 0, 5000);
    }
}
