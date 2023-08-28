package ru.nsu.skopintsev.server.utils;

import org.w3c.dom.Document;
import ru.nsu.skopintsev.utils.ConfigParser;

import java.util.LinkedList;
import java.util.List;

public class ChatHistory {
    private final LinkedList<Document> messages;
    private static final int MAX_MESSAGES = ConfigParser.CHAT_HISTORY;

    public ChatHistory() {
        messages = new LinkedList<>();
    }

    public void addMessage(Document message) {
        if (messages.size() >= MAX_MESSAGES) {
            messages.removeFirst();
        }
        messages.addLast(message);
    }

    public List<Document> getMessages() {
        return messages;
    }
}
