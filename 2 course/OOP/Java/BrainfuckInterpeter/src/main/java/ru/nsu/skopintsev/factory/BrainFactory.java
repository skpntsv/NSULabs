package ru.nsu.skopintsev.factory;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import ru.nsu.skopintsev.Brainfuck;
import ru.nsu.skopintsev.commands.Commands;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Properties;

public class BrainFactory {
    private static final Logger logger = LogManager.getLogger(Brainfuck.class);
    private final HashMap<String, Commands> commandMap;
    private final Properties config;

    public BrainFactory(FileInputStream configStream) throws IOException {
        commandMap = new HashMap<>();

        config = new Properties();
        config.load(configStream);
    }

    public Commands createBrain(String cmd) {
        if (commandMap.containsKey(cmd)) {
            logger.debug(cmd + " was load from hashmap");

            return commandMap.get(cmd);
        }
        String className = config.getProperty(cmd);
        logger.debug(className + " got from config");
        try {
            Commands commandClass = (Commands) Class.forName(className).getClassLoader().loadClass(className).newInstance();
            commandMap.put(cmd, commandClass);
            logger.info(commandClass + " add to hashmap");

            return commandClass;
        } catch (ClassNotFoundException |
                 InstantiationException | IllegalAccessException  e) {
            logger.error("class not found");
            throw new RuntimeException(e);
        }
    }
}
