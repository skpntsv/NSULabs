package ru.nsu.skopintsev.commands;

import org.junit.Test;
import ru.nsu.skopintsev.Data;

import static org.junit.Assert.assertEquals;

public class DecrementValueTest {

    @Test
    public void testSoloDecrement() {
        byte num = 10;
        Data data = new Data("test");
        DecrementValue cmd = new DecrementValue();

        data.setByte(num);
        cmd.execute(data);

        assertEquals(num - 1, data.getByte());
    }
    @Test
    public void testLargeDecrement() {
        byte num = 11;
        Data data = new Data("test");
        DecrementValue cmd = new DecrementValue();

        data.incrementPtr();
        data.setByte(num);
        for (int i = 0; i < 10; i++) {
            cmd.execute(data);
        }
        assertEquals(num - 10, data.getByte());
    }
}
