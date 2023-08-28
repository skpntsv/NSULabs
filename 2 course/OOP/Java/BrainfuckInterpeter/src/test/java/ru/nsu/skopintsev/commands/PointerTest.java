package ru.nsu.skopintsev.commands;

import org.junit.Test;
import ru.nsu.skopintsev.Data;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;

public class PointerTest {

    @Test
    public void testPointer() {
        String code = "test";
        Data data = new Data(code);
        IncrementPointer incrementPointer = new IncrementPointer();
        DecrementPointer decrementPointer = new DecrementPointer();

        data.setByte((byte) 1);
        incrementPointer.execute(data);
        assertEquals(0, data.getByte());
        assertNotEquals(1, data.getByte());

        data.setByte((byte)2);
        decrementPointer.execute(data);
        assertEquals(1, data.getByte());
    }
}
