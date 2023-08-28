package ru.nsu.skopintsev.server.xmlUtils;

import org.jetbrains.annotations.NotNull;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import ru.nsu.skopintsev.utils.MessageType;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import java.util.List;

public class XMLMessageBuilder {
    private Document doc;
    private final String username;
    public XMLMessageBuilder(MessageType messageType, @NotNull String username, String content, List<String> listusers) {
        this.username = username;
        try {
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            doc = dbf.newDocumentBuilder().newDocument();

            switch (messageType) {
                case message -> message(content);
                case userlogin -> userlogin();
                case userlogout -> userlogout();
                case listusers -> listusers(listusers);
                case error -> error(content);
                case success -> success();
                default -> System.out.println("unknown command");
            }

        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        }
    }

    private void success() {
        Element eventElement = doc.createElement("event");
        eventElement.setAttribute("name", "success");
        doc.appendChild(eventElement);
    }

    private void error(String message) {
        // Создание корневого элемента
        Element eventElement = doc.createElement("event");
        eventElement.setAttribute("name", "error");
        doc.appendChild(eventElement);

        // Добавление элемента name
        Element messageElement = doc.createElement("message");
        messageElement.setTextContent(message);
        eventElement.appendChild(messageElement);

        // Добавление элемента name
        Element nameElement = doc.createElement("name");
        nameElement.setTextContent(username);
        eventElement.appendChild(nameElement);
    }

    private void message(@NotNull String message) {
        // Создание корневого элемента
        Element eventElement = doc.createElement("event");
        eventElement.setAttribute("name", "message");
        doc.appendChild(eventElement);

        // Добавление элемента name
        Element messageElement = doc.createElement("message");
        messageElement.setTextContent(message);
        eventElement.appendChild(messageElement);

        // Добавление элемента name
        Element nameElement = doc.createElement("name");
        nameElement.setTextContent(username);
        eventElement.appendChild(nameElement);
    }

    private void userlogin() {
        // Создание корневого элемента
        Element eventElement = doc.createElement("event");
        eventElement.setAttribute("name", "userlogin");
        doc.appendChild(eventElement);

        // Добавление элемента name
        Element nameElement = doc.createElement("name");
        nameElement.setTextContent(username);
        eventElement.appendChild(nameElement);
    }

    private void userlogout() {
        // Создание корневого элемента
        Element eventElement = doc.createElement("event");
        eventElement.setAttribute("name", "userlogout");
        doc.appendChild(eventElement);

        // Добавление элемента name
        Element nameElement = doc.createElement("name");
        nameElement.setTextContent(username);
        eventElement.appendChild(nameElement);
    }

    private void listusers(List<String> listusers) {
        // Создание корневого элемента
        Element commandElement = doc.createElement("event");
        commandElement.setAttribute("name", "listusers");
        doc.appendChild(commandElement);

        // Добавление элемента listusers
        for (String username: listusers) {
            Element sessionElement = doc.createElement("user");
            sessionElement.setTextContent(username);
            commandElement.appendChild(sessionElement);
        }
    }

    public Document getDoc() {
        return doc;
    }

}

