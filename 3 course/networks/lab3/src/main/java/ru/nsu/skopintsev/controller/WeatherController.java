package ru.nsu.skopintsev.controller;

import okhttp3.OkHttpClient;
import okhttp3.Response;
import okhttp3.ResponseBody;
import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.api.responses.WeatherResponse;
import ru.nsu.skopintsev.api.services.WeatherService;
import ru.nsu.skopintsev.model.Weather;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public class WeatherController {
    private final WeatherService weatherService;
    private final OkHttpClient httpClient;

    public WeatherController() {
        this.httpClient = new OkHttpClient();
        weatherService = new WeatherService();
    }

    public CompletableFuture<Weather> getWeatherByCoords(double lat, double lng) {
        CompletableFuture<Weather> future = new CompletableFuture<>();

        httpClient.newCall(weatherService.getRequest(lat, lng)).enqueue(new okhttp3.Callback() {
            @Override
            public void onResponse(@NotNull okhttp3.Call call, @NotNull Response response) throws IOException {
                if (response.isSuccessful()) {
                    ResponseBody responseBody = response.body();
                    if (responseBody != null) {
                        future.complete(mapWeatherResponseToWeather(weatherService.
                                responseBodyToModel(responseBody)));
                    } else {
                        future.completeExceptionally(new IOException("Empty response body"));
                    }
                } else {
                    future.completeExceptionally(new IOException("Request failed with status code: " + response.code()));
                }
            }

            @Override
            public void onFailure(@NotNull okhttp3.Call call, @NotNull IOException e) {
                future.completeExceptionally(e);
            }
        });

        return future;
    }

    private Weather mapWeatherResponseToWeather(WeatherResponse weatherResponse) {
        Weather weather = new Weather();
        weather.setTemperature(weatherResponse.getMain().getTemp());
        weather.setFeelsLikeTemperature(weatherResponse.getMain().getFeels_like());
        weather.setWindSpeed(weatherResponse.getWind().getSpeed());
        weather.setWindDirection(weatherResponse.getWind().getDeg());
        weather.setPressure(weatherResponse.getMain().getPressure());
        weather.setHumidity(weatherResponse.getMain().getHumidity());
        weather.setDesc(weatherResponse.getWeather().get(0).getDescription());

        return weather;
    }
}
