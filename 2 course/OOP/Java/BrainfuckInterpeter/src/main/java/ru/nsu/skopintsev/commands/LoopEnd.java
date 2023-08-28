package ru.nsu.skopintsev.commands;

import ru.nsu.skopintsev.Data;

public class LoopEnd implements Commands{
    @Override
    public void execute(Data data) {
        if (data.getByte() == 0) {
            data.popStack();
        } else {
            data.setStep(data.peekStack());
        }
    }
}
