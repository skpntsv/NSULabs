package ru.nsu.skopintsev.view.utility;

import ru.nsu.skopintsev.controller.KeyController;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
public class KeyListen implements KeyListener {
    private final KeyController keyController;

    public KeyListen(KeyController keyController) {
        this.keyController = keyController;
    }

    @Override
    public void keyTyped(KeyEvent e) {

    }

    @Override
    public void keyPressed(KeyEvent e) {
        int keyCode = e.getKeyCode();
        playerControl(keyCode, true);
    }

    @Override
    public void keyReleased(KeyEvent e) {
        int keyCode = e.getKeyCode();
        playerControl(keyCode, false);
    }

    private void playerControl(int keyCode, boolean status) {
        switch (keyCode) {
            case (KeyEvent.VK_RIGHT) -> keyController.setRight(status);
            case (KeyEvent.VK_LEFT) -> keyController.setLeft(status);
            case (KeyEvent.VK_UP) -> keyController.setUp(status);
            case (KeyEvent.VK_DOWN) -> keyController.setDown(status);
            case (KeyEvent.VK_SPACE) -> keyController.setFire(status);
            default -> {
            }
        }
    }
}
