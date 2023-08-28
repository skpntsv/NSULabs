package ru.nsu.skopintsev.commands;

import ru.nsu.skopintsev.Data;

public class Output implements Commands{
    @Override
    public void execute(Data data) {
        System.out.print((char) data.getByte());
    }
}
