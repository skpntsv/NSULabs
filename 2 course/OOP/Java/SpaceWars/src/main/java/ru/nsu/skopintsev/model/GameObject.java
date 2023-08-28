package ru.nsu.skopintsev.model;

import ru.nsu.skopintsev.controller.GameObjectManager;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Timer;
import java.util.TimerTask;

public abstract class GameObject {
    protected int width;
    protected int height;
    protected BufferedImage bufferedImage;
    protected int x;
    protected int y;
    protected int health;
    protected int speed;
    protected int power;
    protected boolean isAlive;
    protected Color color;
    protected boolean isFiring;
    protected int shotDelay;
    private final Timer shotTimer;
    private TimerTask shotTask;

    public GameObject(int x, int y, int width, int height, int health) {
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
        this.health = health;
        isFiring = false;

        shotTimer = new Timer();
        shotTask = null;
        isAlive = true;
    }

    public void update() {
        checkAlive();

        move();
    }

    public abstract void move();

    public boolean isCollidedWith(GameObject other) {
        return this.x < other.x + other.width &&
                this.x + this.width > other.x &&
                this.y < other.y + other.height &&
                this.y + this.height > other.y;
    }

    public abstract void onCollision(GameObject other);

    public void hit(int damage) {
        health -= damage;
    }

    public boolean isAlive() {
        return isAlive;
    }

    private void checkAlive() {
        isAlive = health > 0 && x >= -100 && x <= 600 && y >= -200 && y <= 900;
    }

    public abstract Shot createShot();

    public BufferedImage getBufferedImage() {
        return bufferedImage;
    }

    public void takeShot(GameObjectManager gameObjectManager) {
        if (shotTask == null && isFiring) {
            shotTask = new TimerTask() {
                @Override
                public void run() {
                    if (!isAlive) {
                        shotTask.cancel();
                        return;
                    }
                    Shot shot = createShot();
                    if (shot != null) {
                        gameObjectManager.addGameObject(shot);
                    }
                    shotTask = null;
                }
            };
            shotTimer.schedule(shotTask, shotDelay);
        }
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public Color getColor() {
        return color;
    }

    protected void loadImage(String imagePath) {
        try (InputStream inputStream = new FileInputStream(imagePath)) {
            bufferedImage = ImageIO.read(inputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
