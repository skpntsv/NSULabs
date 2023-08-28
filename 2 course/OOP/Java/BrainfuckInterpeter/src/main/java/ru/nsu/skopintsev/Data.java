package ru.nsu.skopintsev;

import java.util.ArrayList;

public class Data {
    private final byte[] array;
    private int ptr;
    private int step;
    private final ArrayList<Integer> list;
    private final String code;

    public Data(String code) {
        array = new byte[30_000];
        ptr = 0;
        step = 0;
        list = new ArrayList<>();
        this.code = code;
    }

    public byte getByte() {
        return array[ptr];
    }

    public void setByte(byte num) {
        this.array[ptr] = num;
    }

    public void incrementPtr() {
        ptr++;
    }
    public void decrementPtr() {
        ptr--;
    }

    public int getStep() {
        return step;
    }

    public void setStep(int step) {
        this.step = step;
    }

    public void incrementStep() {
        step++;
    }

    public void pushStack(int step) {
        list.add(step);
    }

    public void popStack() {
        if (list.isEmpty()) {
            throw new RuntimeException();
        }
        list.remove(list.size() - 1);
    }

    public Integer peekStack() {
        if (list.isEmpty()) {
            throw new RuntimeException();
        }
        return list.get(list.size() - 1);
    }

    public String getCode() {
        return code;
    }
}