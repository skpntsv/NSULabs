package ru.nsu.skopintsev;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Main {
    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            System.err.println("Первый аргумент: код на brainfuck\n" +
                    "Второй аргумент: конфиг файл");
            return;
        }
        String inputFile = args[0];
        String config = args[1];
        String code = new String(Files.readAllBytes(Paths.get(inputFile)));
        Brainfuck interpret = new Brainfuck(config, code);
        interpret.interpret();

    }
}
