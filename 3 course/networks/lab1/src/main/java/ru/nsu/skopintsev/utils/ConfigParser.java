package ru.nsu.skopintsev.utils;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

public class ConfigParser {
    private static final String PROPERTIES_FILE = "Chat/src/main/resources/config.properties";
    public static int PORT;
    public static String IP;

    static {
        try (FileInputStream propertiesFile = new FileInputStream(PROPERTIES_FILE)) {
            Properties properties = new Properties();
            properties.load(propertiesFile);

            PORT = Integer.parseInt(properties.getProperty("PORT"));
            IP = properties.getProperty("IP");
        } catch (FileNotFoundException ex) {
            System.err.println("Properties config file not found");
        } catch (IOException ex) {
            System.err.println("Error while reading file");
        }
    }
}