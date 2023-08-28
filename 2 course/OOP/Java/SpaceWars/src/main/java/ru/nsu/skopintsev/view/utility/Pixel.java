package ru.nsu.skopintsev.view.utility;

import java.awt.*;

public class Pixel {
    private static final double DEFAULT_PPI = 144.0;
    private double ppi;

    public Pixel() {
        getScreenPPI();

        System.out.println("Monitor PPI: " + ppi);
    }
    public int pixelScale(int pixels) {
        double scaleFactor = DEFAULT_PPI / ppi;
        return (int) (pixels * scaleFactor);
    }

    private void getScreenPPI() {
        double screenWidth = Toolkit.getDefaultToolkit().getScreenSize().getWidth(); // Ширина монитора в пикселях
        double screenHeight = Toolkit.getDefaultToolkit().getScreenSize().getHeight(); // Высота монитора в пикселях
        double physicalWidthInInches = getPhysicalScreenWidthInInches(); // Физическая ширина монитора в дюймах
        double physicalHeightInInches = getPhysicalScreenHeightInInches(); // Физическая высота монитора в дюймах

        double diagonalInches = Math.sqrt(Math.pow(physicalWidthInInches, 2) + Math.pow(physicalHeightInInches, 2));
        ppi = Math.sqrt(Math.pow(screenWidth, 2) + Math.pow(screenHeight, 2)) / diagonalInches;
    }

    private double getPhysicalScreenWidthInInches() {
        double screenWidth = Toolkit.getDefaultToolkit().getScreenSize().getWidth();
        double screenResolution = Toolkit.getDefaultToolkit().getScreenResolution();
        return screenWidth / screenResolution;
    }

    private double getPhysicalScreenHeightInInches() {
        double screenHeight = Toolkit.getDefaultToolkit().getScreenSize().getHeight();
        double screenResolution = Toolkit.getDefaultToolkit().getScreenResolution();
        return screenHeight / screenResolution;
    }
}
