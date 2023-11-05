package ru.nsu.skopintsev.model;

import lombok.Data;

@Data
public class Location {
    private Double lat;
    private Double lng;
    private String name;
    private String address;
}
