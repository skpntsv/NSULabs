package ru.nsu.skopintsev.controller;

import ru.nsu.skopintsev.api.responses.LocationResponse;
import ru.nsu.skopintsev.api.services.LocationService;

import java.util.concurrent.CompletableFuture;

public class LocationController {
    private final LocationService locationService;

    public LocationController() {
        this.locationService = new LocationService();
    }

    public CompletableFuture<LocationResponse.Location[]> searchLocations(String query) {
        return locationService.searchLocations(query);
    }
}