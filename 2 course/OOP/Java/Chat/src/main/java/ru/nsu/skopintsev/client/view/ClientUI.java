package ru.nsu.skopintsev.client.view;

import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.client.controller.ClientController;

import javax.swing.*;
import java.awt.*;
import java.util.List;

public class ClientUI extends JFrame {
    private final ClientController clientController;
    private String nickname;
    private JTextArea chatArea;
    private JTextField messageField;
    private JTextField nameField;
    private JButton sendButton;
    private JList<String> userList;

    public ClientUI(ClientController clientController) {
        this.clientController = clientController;

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setTitle("Chat Application");

        initUI();
    }

    private void initUI() {
        chatArea = new JTextArea();
        chatArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(chatArea);

        messageField = new JTextField();
        messageField.setEnabled(false);
        messageField.addActionListener(e -> sendMessage());

        sendButton = new JButton("Send");
        sendButton.setEnabled(false);
        sendButton.addActionListener(e -> sendMessage());

        nameField = new JTextField();
        nameField.addActionListener(e -> joinChat());

        userList = new JList<>();
        JScrollPane userListScrollPane = new JScrollPane(userList);
        userListScrollPane.setPreferredSize(new Dimension(100, 0));

        JPanel inputPanel = new JPanel();
        inputPanel.setLayout(new BorderLayout());
        inputPanel.add(messageField, BorderLayout.CENTER);
        inputPanel.add(sendButton, BorderLayout.EAST);

        JPanel namePanel = new JPanel();
        namePanel.setLayout(new BorderLayout());
        namePanel.add(new JLabel("Enter your name: "), BorderLayout.WEST);
        namePanel.add(nameField, BorderLayout.CENTER);

        JPanel menuPanel = new JPanel();
        menuPanel.setLayout(new BorderLayout());
        menuPanel.add(namePanel, BorderLayout.NORTH);
        menuPanel.add(inputPanel, BorderLayout.SOUTH);

        JPanel rightPanel = new JPanel();
        rightPanel.setLayout(new BorderLayout());
        rightPanel.add(userListScrollPane, BorderLayout.CENTER);

        setLayout(new BorderLayout());
        add(scrollPane, BorderLayout.CENTER);
        add(menuPanel, BorderLayout.SOUTH);
        add(rightPanel, BorderLayout.EAST);

        setSize(800,600);
        setLocationRelativeTo(null);
    }

    public void start() {
        setVisible(true);
        nameField.requestFocusInWindow();
    }

    private void joinChat() {
        String username = nameField.getText();
        if (!username.isEmpty()) {
            nameField.setEnabled(false);
            readyToChatting();

            nickname = username;
            clientController.registration();
        }
    }

    private void readyToChatting() {
        messageField.requestFocusInWindow();
        messageField.setEnabled(true);
        sendButton.setEnabled(true);
    }

    public String getNickname() {
        return nickname;
    }

    private void sendMessage() {
        String content = messageField.getText().trim();
        if (!content.equals("")) {
            clientController.sendMessage(content);
        }
        messageField.setText("");
    }

    public void displayMessage(@NotNull String message) {
        chatArea.append(message + "\n");
        chatArea.setCaretPosition(chatArea.getDocument().getLength());
    }

    public void restartReg() {
        nameField.requestFocusInWindow();
        messageField.setEnabled(false);
        sendButton.setEnabled(false);

        nameField.setEnabled(true);
    }

    public void updateUserList(List<String> users) {
        DefaultListModel<String> model = new DefaultListModel<>();
        for (String user : users) {
            model.addElement(user);
        }
        userList.setModel(model);
    }

    public void startTimer() {
        clientController.startTimer();
    }
}
