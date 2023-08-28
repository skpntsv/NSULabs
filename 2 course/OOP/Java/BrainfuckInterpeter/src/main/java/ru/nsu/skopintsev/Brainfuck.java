package ru.nsu.skopintsev;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import ru.nsu.skopintsev.commands.Commands;
import ru.nsu.skopintsev.factory.BrainFactory;

import java.io.FileInputStream;
import java.io.IOException;

public class Brainfuck {
    private static final Logger logger = LogManager.getLogger(Brainfuck.class);
    private final Data data;
    private final BrainFactory factory;
    public Brainfuck(String config, String code) throws IOException {
        data = new Data(code);
        try (FileInputStream configStream = new FileInputStream(config)) {
            factory = new BrainFactory(configStream);
        }
    }

    public void interpret() throws IOException {
        logger.info("Brainfuck interpreter was started");
        while(data.getStep() < data.getCode().length()) {
            if (!Character.isWhitespace(data.getCode().charAt(data.getStep()))) {
                Commands cmd = factory.createBrain(Character.toString(data.getCode().charAt(data.getStep())));

                logger.debug("The command " + cmd + " was started");
                cmd.execute(data);
                logger.debug("The command " + cmd + " was completed");
            }
            data.incrementStep();
        }
        logger.info("Brainfuck interpreter was finished");
    }
}
