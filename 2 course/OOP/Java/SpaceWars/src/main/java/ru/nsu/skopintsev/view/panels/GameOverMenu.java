package ru.nsu.skopintsev.view.panels;

import ru.nsu.skopintsev.view.MainFrame;
import ru.nsu.skopintsev.view.utility.SpaceButtonUI;

import javax.swing.*;
import java.awt.*;

public class GameOverMenu extends JPanel {
    private final MainFrame mainFrame;
    private int buttonHeight = 50;
    private int buttonWidth = 200;

    public GameOverMenu(MainFrame mainFrame) {
        this.mainFrame = mainFrame;

        buttonHeight = goToPixel(buttonHeight);
        buttonWidth = goToPixel(buttonWidth);

        initUI();
    }

    private void initUI() {
        setLayout(new BorderLayout());
        setBackground(Color.BLACK);

        createButtonPanel();
        createTitleLabel();
    }

    private void createTitleLabel() {
        JLabel titleLabel = new JLabel("Game Over");

        titleLabel.setFont(new Font("Arial", Font.BOLD, goToPixel(48)));
        titleLabel.setForeground(Color.RED);
        titleLabel.setHorizontalAlignment(SwingConstants.CENTER);
        titleLabel.setVerticalAlignment(SwingConstants.CENTER);

        add(titleLabel, BorderLayout.CENTER);
    }

    private void createButtonPanel() {
        JPanel buttonPanel = new JPanel(new GridBagLayout());
        buttonPanel.setBackground(Color.BLACK);
        addButtonsToPanel(buttonPanel);
        add(buttonPanel, BorderLayout.SOUTH);
    }

    private void addButtonsToPanel(JPanel buttonPanel) {
        int bottom = goToPixel(10);

        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.insets = new Insets(bottom * 2, 0, bottom, 0);
        buttonPanel.add(createMenuButton(), gbc);

        gbc.gridy = 1;
        gbc.insets = new Insets(bottom, 0, bottom, 0);
        buttonPanel.add(createRestartButton(), gbc);

        gbc.gridy = 2;
        gbc.insets = new Insets(bottom, 0, bottom, 0);
        buttonPanel.add(createExitButton(), gbc);
    }

    private JButton createRestartButton() {
        JButton startButton = new JButton("Restart Game");
        startButton.setPreferredSize(new Dimension(buttonWidth, buttonHeight));
        startButton.setUI(new SpaceButtonUI());
        startButton.addActionListener(e -> restartGame());
        return startButton;
    }

    private JButton createMenuButton() {
        JButton scoresButton = new JButton("Menu");
        scoresButton.setPreferredSize(new Dimension(buttonWidth, buttonHeight));
        scoresButton.setUI(new SpaceButtonUI());
        scoresButton.addActionListener(e -> initMainMenu());
        return scoresButton;
    }

    private JButton createExitButton() {
        JButton exitButton = new JButton("Exit");
        exitButton.setPreferredSize(new Dimension(buttonWidth, buttonHeight));
        exitButton.setUI(new SpaceButtonUI());
        exitButton.addActionListener(e -> exitGame());
        return exitButton;
    }

    private void restartGame() {
        mainFrame.startGame();
    }

    private void initMainMenu() {
        mainFrame.initMainMenu();
    }

    private void exitGame() {
        mainFrame.exitGame();
    }

    private int goToPixel(int pixels) {
        return mainFrame.goToPixel(pixels);
    }
}
