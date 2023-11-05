package ru.nsu.skopintsev.model;

import lombok.Data;

@Data
public class LocationResponse {
    private Location[] hits;
    private long took;

    @Data
    public static class Location {
        private Point point;

        private String name;
        private String country;
        private String city;
        private String state;
        private String street;
        private String housenumber;
        private String postcode;
    }

    @Data
    public static class Point {
        private double lat;
        private double lng;
    }
}
