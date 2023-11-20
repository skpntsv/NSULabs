package ru.nsu.skopintsev.model;

import lombok.Data;

import java.util.List;

@Data
public class Result {
    Location location;
    List<Place> places;
    Weather weather;

    public Result(Location location, List<Place> places) {
        this.location = location;
        this.places = places;
        this.weather = null;
    }

    public Result(Location selectedLocation, Weather weather, List<Place> places) {
        this.location = selectedLocation;
        this.places = places;
        this.weather = weather;
    }
}
