package ru.nsu.skopintsev.view.panels;

import ru.nsu.skopintsev.controller.GameController;
import ru.nsu.skopintsev.model.GameObject;
import ru.nsu.skopintsev.model.objects.Laser;
import ru.nsu.skopintsev.view.utility.KeyListen;
import ru.nsu.skopintsev.view.MainFrame;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.Random;

public class GameSpace extends JPanel {
    public static final int WIDTH = 400;
    public static final int HEIGHT = 700;
    private final MainFrame mainFrame;
    private final GameController gameController;
    private final ImageIcon backgroundImage;
    private Image shipBoomImage;
    private Image laserBoomImage;
    private static final String[] backgroundPaths = {
            "SpaceWars/src/main/resources/picture/backgrounds/background1.jpg",
            "SpaceWars/src/main/resources/picture/backgrounds/background2.jpg",
            "SpaceWars/src/main/resources/picture/backgrounds/background3.jpg",
            "SpaceWars/src/main/resources/picture/backgrounds/background4.jpg",
            "SpaceWars/src/main/resources/picture/backgrounds/background5.jpg",
            "SpaceWars/src/main/resources/picture/backgrounds/background6.jpg"
    };

    public GameSpace(MainFrame mainFrame) {
        gameController = new GameController(this);
        this.mainFrame = mainFrame;

        Random random = new Random();
        int index = random.nextInt(backgroundPaths.length);
        backgroundImage = new ImageIcon(backgroundPaths[index]);
        loadShipBoomImage("SpaceWars/src/main/resources/picture/booms/ship_boom.png");
        loadLaserBoomImage("SpaceWars/src/main/resources/picture/booms/laser_boom.png");

        addKeyListener(new KeyListen(gameController.getKeyController()));

        setFocusable(true);
    }

    public void startGame() {
        gameController.startGame();
    }

    public void finishGame() {
        mainFrame.initGameOverMenu();
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);

        printBackGround(g);

        List<GameObject> gameObjects = gameController.getGameObjects();
        for (GameObject gameObject : gameObjects) {
            if (gameObject.isAlive()) {
                if (gameObject instanceof Laser) {
                    g.setColor(gameObject.getColor());
                    g.fillRect(goToPixel(gameObject.getX()), goToPixel(gameObject.getY()),
                            goToPixel(gameObject.getWidth()), goToPixel(gameObject.getHeight()));
                } else {
                    int width = goToPixel(gameObject.getWidth());
                    int height = goToPixel(gameObject.getHeight());
                    Image image = gameObject.getBufferedImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
                    g.drawImage(image, goToPixel(gameObject.getX()), goToPixel(gameObject.getY()), null);
                }
            } else {
                if (gameObject instanceof Laser) {
                    int x = goToPixel(gameObject.getX()) - laserBoomImage.getHeight(null) / 2;
                    int y = goToPixel(gameObject.getY()) - laserBoomImage.getWidth(null) / 2;
                    g.drawImage(laserBoomImage, x, y, null);
                } else {
                    g.drawImage(shipBoomImage, goToPixel(gameObject.getX()), goToPixel(gameObject.getY()), null);
                }
            }
        }

        drawStatusBar(g);
    }

    private void printBackGround(Graphics g) {
        int imageWidth = backgroundImage.getIconWidth();
        int imageHeight = backgroundImage.getIconHeight();

        int targetWidth = goToPixel(WIDTH);
        int targetHeight = goToPixel(HEIGHT);

        int srcX = (imageWidth - targetWidth) / 2;
        int srcY = (imageHeight - targetHeight) / 2;

        Image image = backgroundImage.getImage();
        g.drawImage(image, 0, 0, targetWidth, targetHeight, srcX, srcY, srcX + targetWidth, srcY + targetHeight, null);
    }

    private void drawStatusBar(Graphics g) {
        int x = goToPixel(10);
        int y = goToPixel(HEIGHT - 50);
        int size = goToPixel(20);
        g.setColor(Color.WHITE);
        g.setFont(new Font("Arial", Font.BOLD, size));
        g.drawString("Score: " + gameController.getPlayerScore(), x, y);

        x = goToPixel(WIDTH - 135);
        g.setColor(Color.WHITE);
        g.setFont(new Font("Arial", Font.BOLD, size));
        g.drawString("Health: " + gameController.getPlayerHealth(), x, y);
    }

    public String enterName(String message, int maxLength) {
        while (true) {
            String name = JOptionPane.showInputDialog(message);

            if (name == null) {
                return null;
            }

            name = name.trim();

            if (name.isEmpty()) {
                JOptionPane.showMessageDialog(null, "Name cannot be empty. Please enter a valid name.");
                continue;
            }

            if (name.length() > maxLength) {
                JOptionPane.showMessageDialog(null, "Name is too long. Please enter a name with maximum " + maxLength + " characters.");
                continue;
            }

            return name;
        }
    }



    private void loadLaserBoomImage(String imagePath) {
        int width = goToPixel(50);
        int height = goToPixel(50);
        try (InputStream inputStream = new FileInputStream(imagePath)) {
            BufferedImage bufferedImage = ImageIO.read(inputStream);
            laserBoomImage = bufferedImage.getScaledInstance(width, height, Image.SCALE_SMOOTH);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void loadShipBoomImage(String imagePath) {
        int width = goToPixel(150);
        int height = goToPixel(150);
        try (InputStream inputStream = new FileInputStream(imagePath)) {
            BufferedImage bufferedImage = ImageIO.read(inputStream);
            shipBoomImage = bufferedImage.getScaledInstance(width, height, Image.SCALE_SMOOTH);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private int goToPixel(int pixels) {
        return mainFrame.goToPixel(pixels);
    }
}
