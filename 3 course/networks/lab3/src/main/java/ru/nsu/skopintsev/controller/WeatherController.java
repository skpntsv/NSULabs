package ru.nsu.skopintsev.controller;

import ru.nsu.skopintsev.api.responses.WeatherResponse;
import ru.nsu.skopintsev.api.services.WeatherService;

import java.util.concurrent.CompletableFuture;

public class WeatherController {
    private WeatherService weatherService;

    public WeatherController() {
        weatherService = new WeatherService();
    }

    public CompletableFuture<WeatherResponse> getWeatherByCoordinates(double latitude, double longitude) {
        return weatherService.getWeatherByCoordinates(latitude, longitude);
    }
}
