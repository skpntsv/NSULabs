package ru.nsu.skopintsev.commands;

import ru.nsu.skopintsev.Data;

import java.io.IOException;

public class Input implements Commands{
    @Override
    public void execute(Data data) throws IOException {
        byte sym = (byte) System.in.read();
        data.setByte(sym);
    }
}
