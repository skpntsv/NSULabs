package ru.nsu.skopintsev.controller;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;
import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.api.responses.WeatherResponse;
import ru.nsu.skopintsev.api.services.WeatherService;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public class WeatherController {
    private WeatherService weatherService;
    private final OkHttpClient httpClient;

    public WeatherController() {
        this.httpClient = new OkHttpClient();
        weatherService = new WeatherService();
    }

    public CompletableFuture<WeatherResponse> getWeatherByCoordinates(double latitude, double longitude) {
        CompletableFuture<WeatherResponse> future = new CompletableFuture<>();

        httpClient.newCall(weatherService.getRequest(latitude, longitude)).enqueue(new okhttp3.Callback() {
            @Override
            public void onResponse(@NotNull okhttp3.Call call, @NotNull Response response) throws IOException {
                if (response.isSuccessful()) {
                    ResponseBody responseBody = response.body();
                    if (responseBody != null) {
                        future.complete(weatherService.responseBodyToModel(responseBody));
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
}
