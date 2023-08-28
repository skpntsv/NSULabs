package ru.nsu.skopintsev;

import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThrows;

public class DataTest {

    @Test
    public void testGetArray() {
        Data data = new Data("test");

        assertEquals(0, data.getByte());

        data.setByte((byte) 42);
        assertEquals(42, data.getByte());
    }

    @Test
    public void testGetSetStep() {
        Data data = new Data("test");

        assertEquals(0, data.getStep());

        data.setStep(5);
        assertEquals(5, data.getStep());
    }

    @Test
    public void testIncrementStep() {
        Data data = new Data("test");

        assertEquals(0, data.getStep());

        data.incrementStep();
        assertEquals(1, data.getStep());
    }

    @Test
    public void testPushStack() {
        Data data = new Data("test");
        int step1 = 1;
        int step2 = 2;
        int step3 = 3;

        data.pushStack(step1);
        assertEquals(Integer.valueOf(step1), data.peekStack());

        data.pushStack(step2);
        assertEquals(Integer.valueOf(step2), data.peekStack());

        data.pushStack(step3);
        assertEquals(Integer.valueOf(step3), data.peekStack());
    }

    @Test
    public void testPopStack() {
        Data data = new Data("test");
        int step1 = 1;
        int step2 = 2;
        int step3 = 3;

        data.pushStack(step1);
        data.pushStack(step2);
        data.pushStack(step3);

        data.popStack();
        assertEquals(Integer.valueOf(step2), data.peekStack());

        data.popStack();
        assertEquals(Integer.valueOf(step1), data.peekStack());
    }

    @Test
    public void testPopAndPeekStackOnEmptyStack() {
        Data data = new Data("test");

        assertThrows(RuntimeException.class, () -> data.popStack());
        assertThrows(RuntimeException.class, () -> data.peekStack());
    }

    @Test
    public void testPeekStack() {
        Data data = new Data("test");

        data.pushStack(1);
        assertEquals((Integer) 1, data.peekStack());

        data.pushStack(2);
        assertEquals((Integer) 2, data.peekStack());
    }

    @Test
    public void testGetCode() {
        String codeExpected = "test";
        Data data = new Data(codeExpected);

        String codeActual = data.getCode();
        assertEquals(codeExpected, codeActual);
    }
}
