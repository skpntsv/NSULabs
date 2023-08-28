package ru.nsu.skopintsev.utils;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

public class ConfigParser {
    private static final String PROPERTIES_FILE = "Chat/src/main/resources/config.properties";
    public static int PORT;
    public static String HOSTNAME;
    public static int CHAT_HISTORY;

    static {
        try (FileInputStream propertiesFile = new FileInputStream(PROPERTIES_FILE)) {
            Properties properties = new Properties();
            properties.load(propertiesFile);

            PORT = Integer.parseInt(properties.getProperty("PORT"));
            HOSTNAME = properties.getProperty("HOSTNAME");
            CHAT_HISTORY = Integer.parseInt(properties.getProperty("CHAT_HISTORY"));
        } catch (FileNotFoundException ex) {
            System.err.println("Properties config file not found");
        } catch (IOException ex) {
            System.err.println("Error while reading file");
        }
    }
}
