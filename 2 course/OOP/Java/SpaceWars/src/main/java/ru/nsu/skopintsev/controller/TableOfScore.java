package ru.nsu.skopintsev.controller;

import java.io.*;
import java.util.*;

public class TableOfScore {
    private final static String filePathToScoreBoard = "SpaceWars/src/main/resources/newScoreBoard.properties";
    private final Map<String, Integer> scoreTable;
    private static final int MAX_LENGTH_USERNAME = 13;

    public TableOfScore() {
        this.scoreTable = loadScoreTable();
    }

    private Map<String, Integer> loadScoreTable() {
        Properties properties = new Properties();
        Map<String, Integer> scoreTable = new LinkedHashMap<>();

        try {
            File file = new File(filePathToScoreBoard);
            if (!file.createNewFile()) {
                try (FileInputStream inputStream = new FileInputStream(file)) {
                    properties.load(inputStream);
                    for (String key : properties.stringPropertyNames()) {
                        int score = Integer.parseInt(properties.getProperty(key));
                        scoreTable.put(key, score);
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return scoreTable;
    }

    public void addScore(String username, int score) {
        scoreTable.put(username, score);
        saveScoreTable();
    }

    private void saveScoreTable() {
        Properties properties = new Properties();
        for (Map.Entry<String, Integer> entry : scoreTable.entrySet()) {
            properties.setProperty(entry.getKey(), String.valueOf(entry.getValue()));
        }

        try {
            try (FileOutputStream outputStream = new FileOutputStream(filePathToScoreBoard)) {
                properties.store(outputStream, "Score board");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public boolean isHighestScore(int score) {
        if (scoreTable.isEmpty()) {
            return true;
        }
        int maxScore = Collections.max(scoreTable.values());
        return score > maxScore;
    }

    public List<Map.Entry<String, Integer>> getTopTen() {
        List<Map.Entry<String, Integer>> entries = new ArrayList<>(scoreTable.entrySet());
        entries.sort(Map.Entry.comparingByValue(Comparator.reverseOrder()));


        int maxEntries = Math.min(entries.size(), 10); // Отдать только топ 10 игроков
        return entries.subList(0, maxEntries);
    }

    public int getMaxLengthUsername() {
        return MAX_LENGTH_USERNAME;
    }
}
