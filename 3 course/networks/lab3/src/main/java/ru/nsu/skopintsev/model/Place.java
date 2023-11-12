package ru.nsu.skopintsev.model;

import lombok.Data;

@Data
public class Place {
    private double lat;
    private double lng;
    private String name;

    private String description;

    @Override
    public String toString() {
        return "\tМесто: " + name +
                "\n\tDescription: " + description;
    }
}
