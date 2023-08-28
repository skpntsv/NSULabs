package ru.nsu.skopintsev.server.controller;

import org.jetbrains.annotations.NotNull;
import org.w3c.dom.Document;
import ru.nsu.skopintsev.utils.ConfigParser;
import ru.nsu.skopintsev.utils.MessageType;
import ru.nsu.skopintsev.server.utils.ChatHistory;
import ru.nsu.skopintsev.server.utils.DataBase;
import ru.nsu.skopintsev.server.xmlUtils.*;
import ru.nsu.skopintsev.utils.XMLUtils;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.List;
import java.util.Set;

public class ChatServer {
    private Selector selector;
    private ServerSocketChannel serverSocketChannel;
    private final DataBase dataBase;
    private final ChatHistory chatHistory;

    public ChatServer() {
        dataBase = new DataBase();
        chatHistory = new ChatHistory();
        try {
            selector = Selector.open();
            serverSocketChannel = ServerSocketChannel.open();
            serverSocketChannel.bind(new InetSocketAddress(ConfigParser.PORT));

            serverSocketChannel.configureBlocking(false);

            serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);
            System.out.println("The server started successfully!");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void start() {
        System.out.println("Listening clients");
        while (true) {
            int readyChannels = 0;
            try {
                readyChannels = selector.select();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
            if (readyChannels > 0) {
                Set<SelectionKey> selectionKeys = selector.selectedKeys();

                selectionKeys.forEach(selectionKey -> {
                    if (selectionKey.isAcceptable()) {
                        acceptHandler(serverSocketChannel);
                    }

                    if (selectionKey.isReadable()) {
                        readHandler(selectionKey);
                    }
                });

                selectionKeys.clear();
            } else {
                System.out.println("Empty");
            }
        }
    }

    private void acceptHandler(@NotNull ServerSocketChannel serverSocketChannel){
        SocketChannel socketChannel;
        try {
            socketChannel = serverSocketChannel.accept();
            System.out.println("accept");
            socketChannel.configureBlocking(false);

            socketChannel.register(selector, SelectionKey.OP_READ);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private void readHandler(@NotNull SelectionKey selectionKey) {
        SocketChannel socketChannel = (SocketChannel) selectionKey.channel();
        ByteBuffer messageBuffer = (ByteBuffer) selectionKey.attachment(); // забрали буффер, если он есть
        if (messageBuffer == null) {
            ByteBuffer sizeBuffer = ByteBuffer.allocate(4);
            int bytesRead = 0;
            while (sizeBuffer.hasRemaining()) {
                try {
                    bytesRead = socketChannel.read(sizeBuffer);
                    if (bytesRead == -1) {
                        disconnectClient(socketChannel);
                        return;
                    }
                } catch (IOException e) {
                    disconnectClient(socketChannel);
                    return;
                }
            }
            if (sizeBuffer.position() == 4) {
                sizeBuffer.flip();
                int messageSize = sizeBuffer.getInt();

                messageBuffer = ByteBuffer.allocate(messageSize);
                selectionKey.attach(messageBuffer); // привязали новый буффер
            }
        }
        try {
            socketChannel.read(messageBuffer);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        if (messageBuffer != null && !messageBuffer.hasRemaining()) {
            messageBuffer.flip();
            messageHandler(messageBuffer, socketChannel);

            selectionKey.attach(null);
        }
    }

    private void messageHandler(ByteBuffer messageBuffer, @NotNull SocketChannel socketChannel) {
        XMLMessageParser xmlParser = new XMLMessageParser(messageBuffer);

        Document messageDocument;
        switch (xmlParser.extractMessageType()) {
            case message -> {
                messageDocument = createMessageDocument(MessageType.message, xmlParser.extractMessageContent(), dataBase.getClientName(socketChannel), null);
                broadcastMessage(messageDocument);
                chatHistory.addMessage(messageDocument);
            }
            case login -> {
                if (dataBase.isFree(xmlParser.extractUsername())) {
                    messageDocument = createMessageDocument(MessageType.userlogin, null, xmlParser.extractUsername(), null);
                    sendChatHistory(socketChannel);
                    broadcastMessage(messageDocument);
                    dataBase.addUser(xmlParser.extractUsername(), socketChannel);
                } else {
                    sendBadName(socketChannel);
                }
            }
            case logout -> {
                messageDocument = createMessageDocument(MessageType.userlogout, null, dataBase.getClientName(socketChannel), null);
                broadcastMessage(messageDocument);
                dataBase.removeUser(dataBase.getClientName(socketChannel));
            }
            case list -> {
                messageDocument = createMessageDocument(MessageType.listusers, null, dataBase.getClientName(socketChannel), dataBase.getUserList());
                sendXMLToSocket(socketChannel, messageDocument);
            }
            default -> {
            }
        }
    }

    private void sendChatHistory(@NotNull SocketChannel socketChannel) {
        Document successMessage = createMessageDocument(MessageType.success, null, "server", null);
        sendXMLToSocket(socketChannel, successMessage);

        List<Document> messages = chatHistory.getMessages();
        for (Document message : messages) {
            sendXMLToSocket(socketChannel, message);
        }
    }

    private void sendBadName(@NotNull SocketChannel socketChannel) {
        Document document = createMessageDocument(MessageType.error, "Pls, change your name", "Bad name", null);

        sendXMLToSocket(socketChannel, document);
        try {
            socketChannel.close();
            socketChannel.keyFor(selector).cancel();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private void sendXMLToSocket(@NotNull SocketChannel socketChannel, @NotNull Document document) {
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
            disconnectClient(socketChannel);
        }
    }


    private Document createMessageDocument(MessageType type, String message, String username, List<String> users) {
        XMLMessageBuilder messageBuilder = new XMLMessageBuilder(type, username, message, users);
        return messageBuilder.getDoc();
    }

    private void broadcastMessage(@NotNull Document document) {
        Set<SelectionKey> selectionKeySet = selector.keys();

        selectionKeySet.forEach(selectionKey -> {
            Channel targetChannel = selectionKey.channel();
            if (targetChannel instanceof SocketChannel dest && targetChannel.isOpen()) {
                sendXMLToSocket(dest, document);
            }
        });
    }

    private void disconnectClient(@NotNull SocketChannel socketChannel) {
        try {
            socketChannel.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        socketChannel.keyFor(selector).cancel();

        String username = dataBase.getClientName(socketChannel);
        String message = String.format("User '%s' has left the chat.", username);
        Document messageDocument = createMessageDocument(MessageType.userlogout, message, dataBase.getClientName(socketChannel), null);
        broadcastMessage(messageDocument);
        System.out.println(message);

        dataBase.removeUser(username);
    }
}
