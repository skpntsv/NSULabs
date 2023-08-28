package ru.nsu.skopintsev.controller;

import ru.nsu.skopintsev.model.*;
import ru.nsu.skopintsev.model.objects.EnemyShip;
import ru.nsu.skopintsev.model.objects.Player;
import ru.nsu.skopintsev.view.panels.GameSpace;

import javax.swing.*;
import java.util.List;

public class GameController implements Runnable {
    private static final int FPS = 33;
    private final GameObjectManager gameObjectManager;
    private final KeyController keyController;
    private Timer enemyTimer;
    private final GameSpace gameSpace;
    private boolean gameOver;
    private int playerScore;
    private int playerHealth;
    private int enemySpawnInterval;

    public GameController(GameSpace gameSpace) {
        this.gameSpace = gameSpace;
        gameObjectManager = new GameObjectManager();
        keyController = new KeyController();
        initializePlayer(keyController);

        enemySpawnInterval = 2500;
        createEnemyTimer();
    }

    private void initializePlayer(KeyController keyController) {
        int playerX = GameSpace.WIDTH / 2 - Player.WIDTH / 2;
        int playerY = GameSpace.HEIGHT - Player.HEIGHT * 2;
        Player player = new Player(playerX, playerY, keyController);
        gameObjectManager.addGameObject(player);
    }

    public void startGame() {
        Thread thread = new Thread(this);
        thread.start();
    }

    public void run() {
        gameOver = false;

        enemyTimer.start();

        long targetFrameTime = 1_000_000_000 / FPS;

        while (!gameOver) {
            long startTime = System.nanoTime();

            gameUpdate();
            gameSpace.repaint();

            long frameTime = System.nanoTime() - startTime;
            long sleepTime = (targetFrameTime - frameTime) / 1_000_000;

            if (sleepTime > 0) {
                try {
                    Thread.sleep(sleepTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        enemyTimer.stop();
        handleGameOver();
    }

    public void gameUpdate() {
        // Обновление игровых объектов и удаление "умервших"
        gameObjectManager.updateGameObjects();

        // Проверка статистики игрока
        updatePlayerStats();

        // Проверка на коллизию
        gameObjectManager.checkCollision();

        // Выполнение действие от объекта (выстрел)
        gameObjectManager.objectExecute();

        // Проверка на конец игры
        gameOver = gameObjectManager.checkGameOver();
    }

    public List<GameObject> getGameObjects() {
        return gameObjectManager.getGameObjects();
    }

    public KeyController getKeyController() {
        return keyController;
    }

    public int getPlayerScore() {
        return playerScore;
    }

    public int getPlayerHealth() {
        return playerHealth;
    }

    private void handleGameOver() {
        TableOfScore tableOfScore = new TableOfScore();
        if (tableOfScore.isHighestScore(playerScore)) {
            String message = "Congratulations! You scored a high score!\nPlease enter your name:";
            String playerName = gameSpace.enterName(message, tableOfScore.getMaxLengthUsername());
            tableOfScore.addScore(playerName, playerScore);
        }

        System.out.println("GAME OVER");
        gameSpace.finishGame();
    }

    private void updatePlayerStats() {
        Player player = gameObjectManager.getPlayer();
        if (player != null) {
            playerScore = player.getScore();
            playerHealth = player.getHealth();
        }
    }

    private void createEnemyTimer() {
        enemyTimer = new Timer(enemySpawnInterval, e -> {
            int x = (int) (Math.random() * (GameSpace.WIDTH - EnemyShip.WIDTH));
            int y = -EnemyShip.HEIGHT;

            while (isOverlappingEnemies(x, y)) {
                x = (int) (Math.random() * (GameSpace.WIDTH - EnemyShip.WIDTH));
            }

            EnemyShip enemyShip = new EnemyShip(x, y);
            gameObjectManager.addGameObject(enemyShip);

            decreaseEnemySpawnInterval();
        });
    }

    private boolean isOverlappingEnemies(int x, int y) {
        List<GameObject> enemies = gameObjectManager.getGameObjects();
        for (GameObject enemy : enemies) {
            if (enemy instanceof EnemyShip && isOverlapping(enemy, x, y)) {
                return true;
            }
        }
        return false;
    }


    private boolean isOverlapping(GameObject obj, int x, int y) {
        int x1 = obj.getX();
        int y1 = obj.getY();
        int x2 = x1 + obj.getWidth();
        int y2 = y1 + obj.getHeight();

        return (x >= x1 && x <= x2 && y >= y1 && y <= y2) ||
                (x + EnemyShip.WIDTH >= x1 && x + EnemyShip.WIDTH <= x2 && y >= y1 && y <= y2) ||
                (x >= x1 && x <= x2 && y + EnemyShip.HEIGHT >= y1 && y + EnemyShip.HEIGHT <= y2) ||
                (x + EnemyShip.WIDTH >= x1 && x + EnemyShip.WIDTH <= x2 && y + EnemyShip.HEIGHT >= y1 && y + EnemyShip.HEIGHT <= y2);
    }

    private void decreaseEnemySpawnInterval() {
        int minSpawnInterval = 500;
        if (enemySpawnInterval > minSpawnInterval) {
            enemySpawnInterval -= 15;
        }
        enemyTimer.setDelay(enemySpawnInterval);
    }

}
