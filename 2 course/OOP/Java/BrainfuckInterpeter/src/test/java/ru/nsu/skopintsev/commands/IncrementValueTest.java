package ru.nsu.skopintsev.commands;

import org.junit.Test;
import ru.nsu.skopintsev.Data;

import static org.junit.Assert.assertEquals;

public class IncrementValueTest {

    @Test
    public void testSoloIncrement() {
        byte num = 10;
        Data data = new Data("test");
        IncrementValue cmd = new IncrementValue();

        data.setByte(num);
        cmd.execute(data);

        assertEquals(num + 1, data.getByte());
    }

    @Test
    public void testLargeIncrement() {
        byte num = 11;
        Data data = new Data("test");
        IncrementValue cmd = new IncrementValue();

        data.incrementPtr();
        data.setByte(num);
        for (int i = 0; i < 10; i++) {
            cmd.execute(data);
        }
        assertEquals(num + 10, data.getByte());
    }
}
