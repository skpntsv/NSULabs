package ru.nsu.skopintsev.factory;

import org.junit.Test;
import ru.nsu.skopintsev.commands.Commands;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

import static org.junit.Assert.*;

public class BrainFactoryTest {
    private static final String testConfigPath = "src/test/resources/test_config.properties";

    @Test
    public void testWithCorrectParams() throws IOException {
        BrainFactory factory;
        try (FileInputStream testConfigStream = new FileInputStream(testConfigPath)) {
            factory = new BrainFactory(testConfigStream);
        }

        Commands cmd = factory.createBrain("+");
        assertNotNull(cmd);
    }

    @Test
    public void testCreateAllCommands() throws IOException {
        BrainFactory factory;
        try (FileInputStream testConfigStream = new FileInputStream(testConfigPath)) {
            factory = new BrainFactory(testConfigStream);
        }

        Properties config = new Properties();
        FileInputStream configStream = new FileInputStream(testConfigPath);
        config.load(configStream);

        for (Object key : config.keySet()) {
            String expectedClassName = config.getProperty((String) key);
            Commands cmd = factory.createBrain((String) key);

            assertNotNull(cmd);
            assertEquals(expectedClassName, cmd.getClass().getName());
        }
    }

    @Test
    public void testHashMap() throws IOException {
        BrainFactory factory;
        try (FileInputStream testConfigStream = new FileInputStream(testConfigPath)) {
            factory = new BrainFactory(testConfigStream);
        }
        Commands cmd = factory.createBrain("+");
        cmd = factory.createBrain("+");
        cmd = factory.createBrain("+");
        cmd = factory.createBrain(">");
        cmd = factory.createBrain(">");
        assertNotNull(cmd);
    }

    @Test
    public void testWithNonExistingCommand() throws IOException {
        try (FileInputStream invalid_testConfigStream = new FileInputStream("src/test/resources/test_invalidconfig.properties")) {
            assertThrows(RuntimeException.class, () -> {
                BrainFactory factory = new BrainFactory(invalid_testConfigStream);
                Commands cmd = factory.createBrain("invalidCmd");
            });
        }
    }
}
