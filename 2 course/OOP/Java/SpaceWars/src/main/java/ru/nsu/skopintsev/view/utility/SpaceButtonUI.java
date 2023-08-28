package ru.nsu.skopintsev.view.utility;

import javax.swing.*;
import javax.swing.plaf.basic.BasicButtonUI;
import java.awt.*;

public class SpaceButtonUI extends BasicButtonUI {
    @Override
    public void paint(Graphics g, JComponent c) {
        AbstractButton button = (AbstractButton) c;
        ButtonModel model = button.getModel();

        if (model.isPressed()) {
            g.setColor(Color.DARK_GRAY);
            g.fillRect(0, 0, button.getWidth(), button.getHeight());
        } else if (model.isRollover()) {
            g.setColor(Color.GRAY);
            g.fillRect(0, 0, button.getWidth(), button.getHeight());
        } else {
            g.setColor(Color.BLACK);
            g.fillRect(0, 0, button.getWidth(), button.getHeight());
        }

        g.setColor(Color.WHITE);
        g.setFont(button.getFont());
        FontMetrics fontMetrics = g.getFontMetrics();
        int x = (button.getWidth() - fontMetrics.stringWidth(button.getText())) / 2;
        int y = (button.getHeight() - fontMetrics.getHeight()) / 2 + fontMetrics.getAscent();
        g.drawString(button.getText(), x, y);
    }
}

