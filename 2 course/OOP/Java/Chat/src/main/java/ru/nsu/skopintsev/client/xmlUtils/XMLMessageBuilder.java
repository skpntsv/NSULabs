package ru.nsu.skopintsev.client.xmlUtils;

import org.jetbrains.annotations.NotNull;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import ru.nsu.skopintsev.utils.MessageType;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

public class XMLMessageBuilder {
    private Document doc;
    private final String username;
    public XMLMessageBuilder(MessageType messageType, @NotNull String username, String content) {
        this.username = username;
        try {
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            doc = dbf.newDocumentBuilder().newDocument();

            switch (messageType) {
                case message -> message(content);
                case list -> list();
                case login -> login();
                case logout -> logout();
                default -> System.out.println("error");
            }

        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        }
    }

    private void message(@NotNull String message) {
        // Создание корневого элемента
        Element commandElement = doc.createElement("command");
        commandElement.setAttribute("name", "message");
        doc.appendChild(commandElement);

        // Добавление элемента name
        Element messageElement = doc.createElement("message");
        messageElement.setTextContent(message);
        commandElement.appendChild(messageElement);

        // Добавление элемента type
        Element sessionElement = doc.createElement("session");
        sessionElement.setTextContent(username);
        commandElement.appendChild(sessionElement);
    }

    private void login() {
        // Создание корневого элемента
        Element commandElement = doc.createElement("command");
        commandElement.setAttribute("name", "login");
        doc.appendChild(commandElement);

        // Добавление элемента name
        Element nameElement = doc.createElement("session");
        nameElement.setTextContent(username);
        commandElement.appendChild(nameElement);

        // Добавление элемента type
        Element typeElement = doc.createElement("type");
        typeElement.setTextContent("CHAT_CLIENT_NAME");
        commandElement.appendChild(typeElement);
    }

    private void logout() {
        // Создание корневого элемента
        Element commandElement = doc.createElement("command");
        commandElement.setAttribute("name", "logout");
        doc.appendChild(commandElement);

        // Добавление элемента session
        Element sessionElement = doc.createElement("session");
        sessionElement.setTextContent(username);
        commandElement.appendChild(sessionElement);
    }

    private void list() {
        // Создание корневого элемента
        Element commandElement = doc.createElement("command");
        commandElement.setAttribute("name", "list");
        doc.appendChild(commandElement);

        // Добавление элемента session
        Element sessionElement = doc.createElement("session");
        sessionElement.setTextContent(username);
        commandElement.appendChild(sessionElement);
    }

    public Document getDoc() {
        return doc;
    }

}

