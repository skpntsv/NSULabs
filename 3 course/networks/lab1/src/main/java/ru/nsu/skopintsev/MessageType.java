package ru.nsu.skopintsev;

enum MessageType {
    REPORT(1),
    LEAVE(2);

    private final int value;

    MessageType(int value) {
        this.value = value;
    }

    public static MessageType fromValue(int value) {
        for (MessageType type : values()) {
            if (type.value == value) {
                return type;
            }
        }
        return null;
    }
}
