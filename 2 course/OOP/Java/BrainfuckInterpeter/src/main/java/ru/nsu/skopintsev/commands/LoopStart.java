package ru.nsu.skopintsev.commands;

import ru.nsu.skopintsev.Data;

public class LoopStart implements Commands{
    @Override
    public void execute(Data data) {
        int loopStart = data.getStep();
        data.pushStack(loopStart);

        if(data.getByte() == 0) {
            String code = data.getCode();
            while(data.getStep() < code.length()) {
                data.incrementStep();
                if (!Character.isWhitespace(code.charAt(data.getStep()))) {
                    if (code.charAt(data.getStep()) == '[') {
                        data.pushStack(data.getStep());
                    }
                    else if (code.charAt(data.getStep()) == ']') {
                        if (data.peekStack() == loopStart) {
                            data.popStack();
                            break;
                        }
                        data.popStack();
                    }
                }
            }
        }
    }
}
