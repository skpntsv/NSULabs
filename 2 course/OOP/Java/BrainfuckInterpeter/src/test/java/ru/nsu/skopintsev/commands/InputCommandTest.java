package ru.nsu.skopintsev.commands;

import org.junit.Test;
import ru.nsu.skopintsev.Data;

import java.io.ByteArrayInputStream;
import java.io.IOException;

import static org.junit.Assert.assertEquals;

public class InputCommandTest {

    @Test
    public void testSoloInput() throws IOException {
        Data data = new Data("test");
        Input cmd = new Input();

        String input = "c";
        ByteArrayInputStream inputStream = new ByteArrayInputStream(input.getBytes());
        System.setIn(inputStream);

        cmd.execute(data);

        byte num = data.getByte();
        assertEquals((byte)input.charAt(0), num);
    }
}
