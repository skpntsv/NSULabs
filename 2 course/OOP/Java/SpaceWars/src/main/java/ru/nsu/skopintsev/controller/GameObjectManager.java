package ru.nsu.skopintsev.controller;

import ru.nsu.skopintsev.model.*;
import ru.nsu.skopintsev.model.objects.EnemyShip;
import ru.nsu.skopintsev.model.objects.Player;

import java.util.ArrayList;
import java.util.List;

public class GameObjectManager {
    private final List<GameObject> gameObjects = new ArrayList<>();

    public void addGameObject(GameObject gameObject) {
        gameObjects.add(gameObject);
    }

    public List<GameObject> getGameObjects() {
        return new ArrayList<>(gameObjects);
    }

    public void objectExecute() {
        List<GameObject> gameObjectsCopy = new ArrayList<>(gameObjects);
        for (GameObject gameObject : gameObjectsCopy) {
            gameObject.takeShot(this);
        }
    }

    public void updateGameObjects() {
        List<GameObject> gameObjectsCopy = new ArrayList<>(gameObjects);
        for (GameObject gameObject : gameObjectsCopy) {
            if (!gameObject.isAlive()) {
                gameObjects.remove(gameObject);
            }

            gameObject.update();
        }
    }

    public void checkCollision() {
        // Проверка на коллизию
        for (int i = 0; i < gameObjects.size(); i++) {
            GameObject gameObjectA = gameObjects.get(i);
            for (int j = i + 1; j < gameObjects.size(); j++) {
                GameObject gameObjectB = gameObjects.get(j);
                if (gameObjectA.isCollidedWith(gameObjectB) &&
                        gameObjectA.isAlive() && gameObjectB.isAlive()) {

                    gameObjectA.onCollision(gameObjectB);
                    gameObjectB.onCollision(gameObjectA);

                    if (gameObjectA instanceof Shot && gameObjectB instanceof EnemyShip) {
                        checkPlayerScores((Shot) gameObjectA, (EnemyShip) gameObjectB);
                    } else if (gameObjectB instanceof Shot && gameObjectA instanceof EnemyShip) {
                        checkPlayerScores((Shot) gameObjectB, (EnemyShip) gameObjectA);
                    }
                }
            }
        }
    }

    public boolean checkGameOver() {
        Player player = getPlayer();
        return player == null;
    }

    public Player getPlayer() {
        for (GameObject gameObject : gameObjects) {
            if (gameObject instanceof Player) {
                return (Player) gameObject;
            }
        }
        return null;
    }

    private void checkPlayerScores(Shot shot, EnemyShip enemyShip) {
        if (shot.getObjectType() == ObjectType.PLAYER && enemyShip.getHealth() < 0) {
            Player player = getPlayer();
            if (player != null) {
                player.increaseScore(enemyShip.getCost());
            }
        }
    }
}

