package ru.nsu.skopintsev.server;

import ru.nsu.skopintsev.server.controller.ChatServer;

public class AppServer {
    public static void main(String[] args) {
        new ChatServer().start();
    }
}