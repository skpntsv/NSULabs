package ru.nsu.skopintsev.commands;

import org.junit.Test;
import ru.nsu.skopintsev.Data;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

import static org.junit.Assert.assertEquals;

public class OutputCommandTest {

    @Test
    public void testSoloOutput() {
        Data data = new Data("test");
        Output cmd = new Output();
        byte num = 57;

        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        System.setOut(new PrintStream(outputStream));

        data.setByte(num);
        cmd.execute(data);

        String result = outputStream.toString();

        assertEquals(Character.toString((char)num), result);
    }
}
