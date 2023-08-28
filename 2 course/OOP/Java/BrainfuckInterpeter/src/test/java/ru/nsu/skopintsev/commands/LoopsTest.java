package ru.nsu.skopintsev.commands;

import org.junit.Test;
import ru.nsu.skopintsev.Data;

import static org.junit.Assert.*;

public class LoopsTest {
    @Test
    public void testLoops_withZeroArray() {
        String code = "[??????]";
        Data data = new Data(code);
        LoopStart loopStart = new LoopStart();

        loopStart.execute(data);

        assertEquals(0, data.getByte());
        assertEquals(code.length() - 1, data.getStep());
        assertThrows(RuntimeException.class, () -> data.peekStack());
    }

    @Test
    public void testLoopStartEnd_withNoZeroArray() {
        Data data = new Data("test");
        LoopStart loopStart = new LoopStart();
        LoopEnd loopEnd = new LoopEnd();

        data.setByte((byte) 1);
        loopStart.execute(data);

        data.incrementPtr();
        data.setByte((byte) 5);
        loopEnd.execute(data);
        assertEquals(0, data.getStep());

        data.incrementPtr();

        loopEnd.execute(data);
        assertThrows(RuntimeException.class, () -> data.peekStack());
    }

    @Test
    public void testLoopStart_returnToLoopStart() {
        Data data = new Data("test");
        LoopStart loopStart = new LoopStart();
        LoopEnd loopEnd = new LoopEnd();
        int step = 2;

        data.setStep(step);
        data.setByte((byte) 1);
        loopStart.execute(data);

        data.incrementPtr();
        data.setByte((byte) 5);
        loopEnd.execute(data);
        assertEquals(step, data.getStep());
    }
}