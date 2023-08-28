package ru.nsu.skopintsev.view;

import ru.nsu.skopintsev.view.panels.GameOverMenu;
import ru.nsu.skopintsev.view.panels.GameSpace;
import ru.nsu.skopintsev.view.panels.HighScorePanel;
import ru.nsu.skopintsev.view.panels.StartMenu;
import ru.nsu.skopintsev.view.utility.Pixel;

import javax.swing.*;
import java.awt.*;

public class MainFrame extends JFrame {
    private static final int WIDTH = 400;
    private static final int HEIGHT = 700;
    private JPanel currentPanel;
    private final Pixel pixel;

    public MainFrame() {
        this.pixel = new Pixel();
        initUI();

        initMainMenu();
    }

    private void initUI() {
        setTitle("Space Wars"); // название окна
        setSize(goToPixel(WIDTH), goToPixel(HEIGHT)); // размер окна
        setBackground(Color.BLACK);
        setResizable(false); // запрещаем редактирование размер окна
        setLocationRelativeTo(null); // позиция окна по центру экрана
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); // закрытие приложения при закрытии окна
        setLayout(new BorderLayout());

        ImageIcon icon = new ImageIcon("SpaceWars/src/main/resources/picture/space_icon.png");
        setIconImage(icon.getImage()); // установка иконки приложения

        setVisible(true);
    }

    public int goToPixel(int pixels) {
        return pixel.pixelScale(pixels);
    }

    public void initGameOverMenu() {
        switchPanel(new GameOverMenu(this));
    }

    public void initMainMenu() {
        switchPanel(new StartMenu(this));
    }

    private void initHighScorePanel() {
        switchPanel(new HighScorePanel(this));
    }

    private void initGameSpacePanel(GameSpace gameSpace) {
        switchPanel(gameSpace);
    }

    public void exitGame() {
        System.exit(0);
    }

    public void showHighScores() {
        initHighScorePanel();
    }

    public void startGame() {
        GameSpace gameSpace = new GameSpace(this);
        initGameSpacePanel(gameSpace);
        gameSpace.startGame();
    }

    private void switchPanel(JPanel newPanel) {
        if (currentPanel != null) {
            remove(currentPanel);
        }

        currentPanel = newPanel;
        add(currentPanel, BorderLayout.CENTER);
        currentPanel.requestFocusInWindow();

        SwingUtilities.invokeLater(() -> {
            revalidate();
            repaint();
        });
    }
}
