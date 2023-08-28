package ru.nsu.skopintsev.model.objects;

import ru.nsu.skopintsev.model.ObjectType;
import ru.nsu.skopintsev.model.Shot;

import java.awt.*;

public class Laser extends Shot {
    public static final int WIDTH = 5;
    public static final int HEIGHT = 15;

    public Laser(int x, int y, ObjectType parentType) {
        super(x, y, WIDTH, HEIGHT, Color.BLUE, parentType);
        
        power = 20;
        if (parentType == ObjectType.PLAYER) {
            speed = 10;
            color = Color.BLUE;
        } else {
            speed = -5;
            color = Color.GREEN;
        }
    }

    public void move() {
        y -= speed;
    }

    @Override
    public Shot createShot() {
        return null;
    }

}
