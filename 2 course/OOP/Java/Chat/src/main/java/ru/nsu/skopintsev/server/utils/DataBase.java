package ru.nsu.skopintsev.server.utils;

import org.jetbrains.annotations.NotNull;

import java.nio.channels.SocketChannel;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class DataBase {
    private final Map<String, SocketChannel> clients;

    public DataBase() {
        this.clients = new HashMap<>();
    }

    public void addUser(String userName, SocketChannel socketChannel) {
        clients.put(userName, socketChannel);
    }

    public String getClientName(SocketChannel socketChannel) {
        for (Map.Entry<String, SocketChannel> entry : clients.entrySet()) {
            if (entry.getValue() == socketChannel) {
                return entry.getKey();
            }
        }
        return null;
    }

    public void removeUser(@NotNull String clientName) {
        clients.remove(clientName);
    }

    public boolean isFree(String clientName) {
        return !clients.containsKey(clientName);
    }

    public List<String> getUserList() {
        return new ArrayList<>(clients.keySet());
    }
}
