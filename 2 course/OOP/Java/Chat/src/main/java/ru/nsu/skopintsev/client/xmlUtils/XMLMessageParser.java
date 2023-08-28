package ru.nsu.skopintsev.client.xmlUtils;

import org.w3c.dom.*;
import org.xml.sax.SAXException;
import ru.nsu.skopintsev.utils.MessageType;

import javax.xml.parsers.*;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class XMLMessageParser {
    private final Document document;

    public XMLMessageParser(ByteBuffer buffer) {
        document = parseXMLBuffer(buffer);
        if (document == null) {
            throw new IllegalArgumentException("Invalid XML buffer");
        }
    }

    private Document parseXMLBuffer(ByteBuffer xmlBuffer) {
        try {
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            DocumentBuilder builder = factory.newDocumentBuilder();
            ByteArrayInputStream inputStream = new ByteArrayInputStream(xmlBuffer.array(), xmlBuffer.position(), xmlBuffer.remaining());
            return builder.parse(inputStream);
        } catch (ParserConfigurationException | SAXException | IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public String extractUsername() {
        Element root = document.getDocumentElement();
        NodeList nodeList = root.getElementsByTagName("name");
        if (nodeList.getLength() > 0) {
            Node nameNode = nodeList.item(0);
            return nameNode.getTextContent();
        }
        return null;
    }

    public MessageType extractMessageType() {
        Element root = document.getDocumentElement();
        String messageTypeString = root.getAttribute("name");
        try {
            return MessageType.valueOf(messageTypeString);
        } catch (IllegalArgumentException e) {
            return null;
        }
    }

    public String extractMessageContent() {
        Element root = document.getDocumentElement();
        NodeList nodeList = root.getElementsByTagName("message");
        if (nodeList.getLength() > 0) {
            Node nameNode = nodeList.item(0);
            return nameNode.getTextContent();
        }
        return null;
    }

    public List<String> extractListOfUsers() {
        List<String> userList = new ArrayList<>();
        NodeList userListNodes = document.getElementsByTagName("user");
        for (int i = 0; i < userListNodes.getLength(); i++) {
            Node userNode = userListNodes.item(i);
            userList.add(userNode.getTextContent());
        }
        return userList;
    }
}
