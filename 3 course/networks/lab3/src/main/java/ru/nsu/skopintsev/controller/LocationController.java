package ru.nsu.skopintsev.controller;

import okhttp3.OkHttpClient;
import okhttp3.Response;
import okhttp3.ResponseBody;
import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.api.responses.LocationResponse;
import ru.nsu.skopintsev.api.services.LocationService;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public class LocationController {
    private final LocationService locationService;
    private final OkHttpClient httpClient;

    public LocationController() {
        this.httpClient = new OkHttpClient();
        this.locationService = new LocationService();
    }

    public CompletableFuture<LocationResponse.Location[]> searchLocations(String query) {
        CompletableFuture<LocationResponse.Location[]> future = new CompletableFuture<>();

        httpClient.newCall(locationService.getRequest(query)).enqueue(new okhttp3.Callback() {
            @Override
            public void onResponse(@NotNull okhttp3.Call call, @NotNull Response response) throws IOException {
                if (response.isSuccessful()) {
                    ResponseBody responseBody = response.body();
                    if (responseBody != null) {
                        LocationResponse locationResponse = locationService.responseBodyToLocationResponse(responseBody);
                        future.complete(locationResponse.getHits());
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