package ru.nsu.skopintsev.view.panels;

import ru.nsu.skopintsev.view.MainFrame;
import ru.nsu.skopintsev.view.utility.SpaceButtonUI;

import javax.swing.*;
import java.awt.*;

public class StartMenu extends JPanel {
    private final MainFrame mainFrame;
    private int buttonHeight = 50;
    private int buttonWidth = 200;

    public StartMenu(MainFrame mainFrame) {
        this.mainFrame = mainFrame;

        buttonHeight = goToPixel(buttonHeight);
        buttonWidth = goToPixel(buttonWidth);

        initUI();
    }

    private void initUI() {
        setLayout(new BorderLayout());
        setBackground(Color.BLACK);

        createTitleLabel();
        createButtonPanel();
    }

    private void createTitleLabel() {
        JLabel titleLabel = new JLabel("Space Wars");

        titleLabel.setFont(new Font("Arial", Font.BOLD, goToPixel(48)));
        titleLabel.setForeground(Color.WHITE);
        titleLabel.setHorizontalAlignment(SwingConstants.CENTER);
        titleLabel.setVerticalAlignment(SwingConstants.CENTER);

        titleLabel.setBorder(BorderFactory.createEmptyBorder(goToPixel(90), 0, 0, 0));

        add(titleLabel, BorderLayout.NORTH);
    }

    private void createButtonPanel() {
        JPanel buttonPanel = new JPanel(new GridBagLayout());
        buttonPanel.setBackground(Color.BLACK);
        addButtonsToPanel(buttonPanel);
        add(buttonPanel, BorderLayout.CENTER);
    }

    private void addButtonsToPanel(JPanel buttonPanel) {
        int bottom = goToPixel(10);

        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.insets = new Insets(goToPixel(20), 0, bottom, 0);
        buttonPanel.add(createStartButton(), gbc);

        gbc.gridy = 1;
        gbc.insets = new Insets(bottom, 0, bottom, 0);
        buttonPanel.add(createScoresButton(), gbc);

        gbc.gridy = 2;
        gbc.insets = new Insets(bottom, 0, bottom, 0);
        buttonPanel.add(createExitButton(), gbc);
    }

    private JButton createStartButton() {
        JButton startButton = new JButton("Start Game");
        startButton.setPreferredSize(new Dimension(buttonWidth, buttonHeight));
        startButton.setUI(new SpaceButtonUI());
        startButton.addActionListener(e -> startGame());
        return startButton;
    }

    private JButton createScoresButton() {
        JButton scoresButton = new JButton("High Scores");
        scoresButton.setPreferredSize(new Dimension(buttonWidth, buttonHeight));
        scoresButton.setUI(new SpaceButtonUI());
        scoresButton.addActionListener(e -> showHighScores());
        return scoresButton;
    }

    private JButton createExitButton() {
        JButton exitButton = new JButton("Exit");
        exitButton.setPreferredSize(new Dimension(buttonWidth, buttonHeight));
        exitButton.setUI(new SpaceButtonUI());
        exitButton.addActionListener(e -> exitGame());
        return exitButton;
    }

    private void startGame() {
        mainFrame.startGame();
    }

    private void showHighScores() {
        mainFrame.showHighScores();
    }

    private void exitGame() {
        mainFrame.exitGame();
    }

    private int goToPixel(int pixels) {
        return mainFrame.goToPixel(pixels);
    }
}
