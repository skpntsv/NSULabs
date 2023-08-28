package ru.nsu.skopintsev.model.objects;

import ru.nsu.skopintsev.controller.KeyController;
import ru.nsu.skopintsev.model.GameObject;
import ru.nsu.skopintsev.model.ObjectType;
import ru.nsu.skopintsev.model.Shot;
import ru.nsu.skopintsev.model.objects.Laser;

import java.awt.*;

public class Player extends GameObject {
    public static final int WIDTH = 100;
    public static final int HEIGHT = 100;
    private static final String imagePath = "SpaceWars/src/main/resources/picture/models/player_ship.png";
    private int score;
    private final KeyController keyController;

    public Player(int x, int y, KeyController keyController) {
        super(x, y, WIDTH, HEIGHT, 100);
        this.keyController = keyController;

        speed = 5;
        score = 0;
        power = 20;
        color = Color.CYAN;
        shotDelay = 250;

        loadImage(imagePath);
    }

    @Override
    public void move() {
        if (keyController.isLeft()) {
            moveLeft();
        }
        if (keyController.isRight()) {
            moveRight();
        }
        if (keyController.isUp()) {
            moveUp();
        }
        if (keyController.isDown()) {
            moveDown();
        }
        isFiring = keyController.isFiring();
    }

    @Override
    public void onCollision(GameObject other) {
        other.hit(power);
    }

    public Shot createShot() {
        return new Laser(x + width / 2 - Laser.WIDTH / 2, y - Laser.HEIGHT, ObjectType.PLAYER);
    }

    public int getScore() {
        return score;
    }

    public void increaseScore(int points) {
        score += points;
    }


    public int getHealth() {
        return health;
    }

    private void moveRight() {
        x += speed;
    }

    private void moveLeft() {
        x -= speed;
    }

    private void moveUp() {
        y -= speed;
    }

    private void moveDown() {
        y += speed;
    }
}
