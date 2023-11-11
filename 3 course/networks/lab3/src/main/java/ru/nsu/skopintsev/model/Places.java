package ru.nsu.skopintsev.model;

import lombok.Data;

@Data
public class Places {
    private double lat;
    private double lng;
    private String name;
    private String address;
    private String imageUrl;

    private String description;
}
