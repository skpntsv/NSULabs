package ru.nsu.skopintsev.model.objects;

import ru.nsu.skopintsev.model.GameObject;
import ru.nsu.skopintsev.model.ObjectType;
import ru.nsu.skopintsev.model.Shot;

import java.awt.*;
import java.util.Random;

public class EnemyShip extends GameObject {
    public static final int WIDTH = 50;
    public static final int HEIGHT = 80;
    private static final String[] imagePaths = {
            "SpaceWars/src/main/resources/picture/models/enemy_ship1.png",
            "SpaceWars/src/main/resources/picture/models/enemy_ship2.png",
            "SpaceWars/src/main/resources/picture/models/enemy_ship3.png"
    };
    private final int cost;

    public EnemyShip(int x, int y) {
        super(x, y, WIDTH, HEIGHT, 100);

        speed = 1;
        color = Color.RED;
        shotDelay = 3000;
        cost = 100;
        health = 50;
        power = 5;

        Random random = new Random();
        int randomIndex = random.nextInt(imagePaths.length);
        String imagePath = imagePaths[randomIndex];
        loadImage(imagePath);
    }

    @Override
    public Shot createShot() {
        return new Laser(x + width / 2 - Laser.WIDTH / 2, y + height, ObjectType.ENEMY);
    }

    public void move() {
        y += speed;

        Random random = new Random();
        isFiring = random.nextBoolean();
    }

    @Override
    public void onCollision(GameObject other) {
        other.hit(power);
    }

    public int getCost() {
        return cost;
    }

    public int getHealth() {
        return health;
    }
}
