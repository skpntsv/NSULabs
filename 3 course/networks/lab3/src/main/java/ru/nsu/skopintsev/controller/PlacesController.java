package ru.nsu.skopintsev.controller;

import ru.nsu.skopintsev.model.Place;
import ru.nsu.skopintsev.api.services.PlacesService;

import java.util.concurrent.CompletableFuture;

public class PlacesController {
    private PlacesService placesService;

    public PlacesController() {
        this.placesService = new PlacesService();
    }

    public CompletableFuture<Place[]> getInterestingPlaces(double lat, double lng) {
        return placesService.getInterestingPlaces(lat, lng);
    }
}