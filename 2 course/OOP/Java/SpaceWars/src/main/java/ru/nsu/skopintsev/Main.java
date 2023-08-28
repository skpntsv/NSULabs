package ru.nsu.skopintsev;

import ru.nsu.skopintsev.view.MainFrame;

import java.awt.*;

public class Main {
    public static void main(String[] args) {
        EventQueue.invokeLater(() -> {

            var ex = new MainFrame();
            ex.setVisible(true);
        });
    }
}