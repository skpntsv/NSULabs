package ru.nsu.skopintsev.controller;

import okhttp3.OkHttpClient;
import okhttp3.Response;
import okhttp3.ResponseBody;
import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.api.responses.PlaceDescriptionResponse;
import ru.nsu.skopintsev.api.services.PlaceDescriptionService;
import ru.nsu.skopintsev.model.Place;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.CompletionException;

public class PlaceDescriptionController {
    private final PlaceDescriptionService placeDescriptionService;
    private final OkHttpClient httpClient;

    public PlaceDescriptionController() {
        this.placeDescriptionService = new PlaceDescriptionService();
        this.httpClient = new OkHttpClient();
    }

    public ArrayList<Place> getPlacesDescriptions(String[] xids) {
        ArrayList<Place> places = new ArrayList<>();
        List<CompletableFuture<Place>> futures = new ArrayList<>();

        for (String xid : xids) {
            CompletableFuture<Place> placeFuture = getOnePlaceDesc(xid);
            futures.add(placeFuture);
        }

        for (CompletableFuture<Place> placeFuture : futures) {
            try {
                Place place = placeFuture.join();
                places.add(place);
            } catch (CompletionException e) {
                //System.err.println("ERROR: " + e.getCause().getMessage());
            }
        }

        return places;
    }

    private CompletableFuture<Place> getOnePlaceDesc(String xid) {
        CompletableFuture<Place> future = new CompletableFuture<>();
        httpClient.newCall(placeDescriptionService.getRequest(xid)).enqueue(new okhttp3.Callback() {
            @Override
            public void onResponse(@NotNull okhttp3.Call call, @NotNull Response response) {
                if (response.isSuccessful()) {
                    try (ResponseBody responseBody = response.body()) {
                        if (responseBody != null) {
                            try {
                                PlaceDescriptionResponse placeDescriptionResponse =
                                        placeDescriptionService.responseBodyToModel(responseBody);

                                if (placeDescriptionResponse.getName() != null
                                        && placeDescriptionResponse.getPoint() != null) {
                                    future.complete(mapPlaceDescriptionResponseToPlace(placeDescriptionResponse));
                                } else {
                                    future.completeExceptionally(new NullPointerException("One of the mandatory fields is null"));
                                }
                            } catch (Exception e) {
                                future.completeExceptionally(e);
                            }
                        } else {
                            future.completeExceptionally(new IOException("Empty response body"));
                        }
                    } finally {
                        assert response.body() != null;
                        response.body().close();
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

    private Place mapPlaceDescriptionResponseToPlace(PlaceDescriptionResponse placeDescriptionResponse) {
        Place place = new Place();
        place.setName(placeDescriptionResponse.getName());
        place.setLat(placeDescriptionResponse.getPoint().getLat());
        place.setLng(placeDescriptionResponse.getPoint().getLon());
        if (placeDescriptionResponse.getInfo() == null) {
            place.setDescription("not stated");
        } else {
            place.setDescription(placeDescriptionResponse.getInfo().getDescr());
        }

        return place;
    }
}
