package ru.nsu.skopintsev.commands;

import ru.nsu.skopintsev.Data;

import java.io.IOException;

public interface Commands {
    void execute(Data data) throws IOException;
}
