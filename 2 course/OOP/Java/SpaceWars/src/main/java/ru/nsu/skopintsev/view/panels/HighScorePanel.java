package ru.nsu.skopintsev.view.panels;

import ru.nsu.skopintsev.controller.TableOfScore;
import ru.nsu.skopintsev.view.MainFrame;
import ru.nsu.skopintsev.view.utility.SpaceButtonUI;

import javax.swing.*;
import java.awt.*;
import java.util.List;
import java.util.Map;

public class HighScorePanel extends JPanel {
    private final MainFrame mainFrame;
    private final TableOfScore scoreTable;

    public HighScorePanel(MainFrame mainFrame) {
        this.mainFrame = mainFrame;
        this.scoreTable = new TableOfScore();

        setBackground(Color.BLACK);
        setLayout(new BorderLayout());

        initUI();
    }

    private void initUI() {
        createTitleLabel();
        createButtonPanel();
        createScorePanel();
    }

    private void createTitleLabel() {
        JLabel titleLabel = new JLabel("High scores");

        titleLabel.setFont(new Font("Arial", Font.BOLD, goToPixel(48)));
        titleLabel.setForeground(Color.WHITE);
        titleLabel.setHorizontalAlignment(SwingConstants.CENTER);
        titleLabel.setVerticalAlignment(SwingConstants.CENTER);

        titleLabel.setBorder(BorderFactory.createEmptyBorder(goToPixel(30), 0, 0, 0));

        add(titleLabel, BorderLayout.NORTH);
    }

    private void createScorePanel() {
        JPanel scorePanel = new JPanel();
        scorePanel.setBackground(Color.BLACK);
        scorePanel.setLayout(new BoxLayout(scorePanel, BoxLayout.Y_AXIS));

        List<Map.Entry<String, Integer>> entries = scoreTable.getTopTen();
        int rank = 1;
        for (Map.Entry<String, Integer> entry : entries) {
            JLabel scoreLabel = new JLabel(String.format("%d. %s - %d", rank, entry.getKey(), entry.getValue()));
            scoreLabel.setFont(new Font("Arial", Font.BOLD, goToPixel(30)));
            scoreLabel.setForeground(Color.WHITE);
            scoreLabel.setAlignmentX(Component.CENTER_ALIGNMENT);

            scoreLabel.setBorder(BorderFactory.createEmptyBorder(goToPixel(5), 0, 0, 0));
            scorePanel.add(scoreLabel, BorderLayout.CENTER);

            rank++;
        }

        scorePanel.setBorder(BorderFactory.createEmptyBorder(goToPixel(50), 0, 0, 0));
        add(scorePanel, BorderLayout.CENTER);
    }

    private void createButtonPanel() {
        JPanel buttonPanel = new JPanel(new GridBagLayout());
        buttonPanel.setBackground(Color.BLACK);
        addButtonsToPanel(buttonPanel);
        add(buttonPanel, BorderLayout.SOUTH);
    }

    private void addButtonsToPanel(JPanel buttonPanel) {
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.insets = new Insets(goToPixel(20), 0, 10, 0);
        buttonPanel.add(createBackButton(), gbc);
    }

    private JButton createBackButton() {
        JButton scoresButton = new JButton("Back");
        scoresButton.setPreferredSize(new Dimension(goToPixel(200), goToPixel(50)));
        scoresButton.setUI(new SpaceButtonUI());
        scoresButton.addActionListener(e -> back());
        return scoresButton;
    }

    private void back() {
        mainFrame.initMainMenu();
    }

    private int goToPixel(int pixels) {
        return mainFrame.goToPixel(pixels);
    }
}
