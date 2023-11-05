package ru.nsu.skopintsev.model;

import lombok.Getter;
import lombok.Setter;

@Setter
@Getter
public class Place {
    private String id;
    private String name;
    private double latitude;
    private double longitude;

    @Override
    public String toString() {
        return "Place{" +
                "id='" + id + '\'' +
                ", name='" + name + '\'' +
                ", latitude=" + latitude +
                ", longitude=" + longitude +
                '}';
    }
}
