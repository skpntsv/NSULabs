package ru.nsu.skopintsev.model;

import ru.nsu.skopintsev.model.objects.EnemyShip;
import ru.nsu.skopintsev.model.objects.Player;

import java.awt.Color;

public abstract class Shot extends GameObject {
    protected ObjectType objectType;

    public Shot(int x, int y, int width, int height, Color color, ObjectType objectType) {
        super(x, y, width, height, 1);
        this.color = color;
        this.objectType = objectType;

    }

    @Override
    public void onCollision(GameObject other) {
        if (matchesParentType(other)) {
            other.hit(power);
        }
    }

    private boolean matchesParentType(GameObject object) {
        if (objectType == ObjectType.ENEMY && object instanceof EnemyShip ||
                objectType == ObjectType.PLAYER && object instanceof Player) {
            return false;
        }
        return true;
    }

    public ObjectType getObjectType() {
        return objectType;
    }
}
