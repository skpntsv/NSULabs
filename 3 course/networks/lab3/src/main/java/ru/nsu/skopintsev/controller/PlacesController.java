package ru.nsu.skopintsev.controller;

import okhttp3.OkHttpClient;
import okhttp3.Response;
import okhttp3.ResponseBody;
import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.api.responses.PlacesResponse;
import ru.nsu.skopintsev.api.services.PlacesService;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public class PlacesController {
    private static final int RADIUS = 100; // in metres
    private final PlacesService placesService;
    private final OkHttpClient httpClient;

    public PlacesController() {
        this.placesService = new PlacesService();
        this.httpClient = new OkHttpClient();
    }

    public CompletableFuture<PlacesResponse> getInterestingPlaces(double lat, double lng) {
        CompletableFuture<PlacesResponse> future = new CompletableFuture<>();

        httpClient.newCall(placesService.getRequest(lat, lng, RADIUS)).enqueue(new okhttp3.Callback() {
            @Override
            public void onResponse(@NotNull okhttp3.Call call, @NotNull Response response) throws IOException {
                if (response.isSuccessful()) {
                    ResponseBody responseBody = response.body();
                    if (responseBody != null) {
                        future.complete(placesService.responseBodyToModel(responseBody));
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