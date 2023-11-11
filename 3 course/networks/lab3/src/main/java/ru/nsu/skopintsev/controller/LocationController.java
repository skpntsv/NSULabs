package ru.nsu.skopintsev.controller;

import okhttp3.OkHttpClient;
import okhttp3.Response;
import okhttp3.ResponseBody;
import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.api.responses.LocationResponse;
import ru.nsu.skopintsev.api.services.LocationService;
import ru.nsu.skopintsev.model.Location;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public class LocationController {
    private final LocationService locationService;
    private final OkHttpClient httpClient;

    public LocationController() {
        this.httpClient = new OkHttpClient();
        this.locationService = new LocationService();
    }

    public CompletableFuture<Location[]> searchLocations(String query) {
        CompletableFuture<Location[]> future = new CompletableFuture<>();

        httpClient.newCall(locationService.getRequest(query)).enqueue(new okhttp3.Callback() {
            @Override
            public void onResponse(@NotNull okhttp3.Call call, @NotNull Response response) throws IOException {
                if (response.isSuccessful()) {
                    ResponseBody responseBody = response.body();
                    if (responseBody != null) {
                        LocationResponse locationResponse = locationService.responseBodyToLocationResponse(responseBody);
                        future.complete(getLocationFromResponse(locationResponse.getHits()));
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

    private Location[] getLocationFromResponse(LocationResponse.Location[] locations) {
        Location[] mappedLocations = new Location[locations.length];
        for (int i = 0; i < locations.length; i++) {
            mappedLocations[i] = mapLocationResponseToLocation(locations[i]);
        }

        return mappedLocations;
    }

    private Location mapLocationResponseToLocation(LocationResponse.Location locationResponse) {
        Location location = new Location();

        location.setLat(locationResponse.getPoint().getLat());
        location.setLng(locationResponse.getPoint().getLng());
        location.setName(locationResponse.getName());

        StringBuilder addressBuilder = new StringBuilder();
        if (locationResponse.getCountry() != null) {
            addressBuilder.append(locationResponse.getCountry()).append(", ");
        }
        if (locationResponse.getState() != null) {
            addressBuilder.append(locationResponse.getState()).append(", ");
        }
        if (locationResponse.getCity() != null) {
            addressBuilder.append(locationResponse.getCity()).append(", ");
        }
        if (locationResponse.getStreet() != null) {
            addressBuilder.append(locationResponse.getStreet()).append(", ");
        }
        if (locationResponse.getHousenumber() != null) {
            addressBuilder.append(locationResponse.getHousenumber()).append(", ");
        }
        if (locationResponse.getPostcode() != null) {
            addressBuilder.append(locationResponse.getPostcode());
        }

        location.setAddress(addressBuilder.toString());

        return location;
    }
}