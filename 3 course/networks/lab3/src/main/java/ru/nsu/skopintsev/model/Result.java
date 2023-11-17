package ru.nsu.skopintsev.model;

import lombok.Data;

@Data
public class Result {
    Location location;
    Place[] places;
    Weather weather;

    public Result(Location selectedLocation, Weather weather, Place[] places) {
        this.location = selectedLocation;
        this.places = places;
        this.weather = weather;
    }
}
