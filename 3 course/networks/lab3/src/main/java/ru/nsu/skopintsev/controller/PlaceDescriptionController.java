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
import java.util.Objects;
import java.util.concurrent.CompletableFuture;

public class PlaceDescriptionController {
    private final PlaceDescriptionService placeDescriptionService;
    private final OkHttpClient httpClient;

    public PlaceDescriptionController() {
        this.placeDescriptionService = new PlaceDescriptionService();
        this.httpClient = new OkHttpClient();
    }

    public CompletableFuture<Place[]> getPlacesDescriptions(String[] xids) {
        List<CompletableFuture<Place>> futures = new ArrayList<>();

        for (String xid : xids) {
            CompletableFuture<Place> future = new CompletableFuture<>();
            futures.add(future);
//            future.exceptionally(ex -> {
//                //System.err.println(ex.getMessage());
//                System.err.println(future.toString());
//                futures.remove(future);
//                return null;
//            });
//            if (future.isDone() && !future.isCompletedExceptionally()) {
//                futures.add(future);
//                System.out.println(future);
//            }
//            if (!future.isCompletedExceptionally()) {
//                futures.add(future);
//                //System.out.println(future.toString());
//            }

            httpClient.newCall(placeDescriptionService.getRequest(xid)).enqueue(new okhttp3.Callback() {
                @Override
                public void onResponse(@NotNull okhttp3.Call call, @NotNull Response response) throws IOException {
                    if (response.isSuccessful()) {
                        ResponseBody responseBody = response.body();
                        if (responseBody != null) {
                            try {
                                PlaceDescriptionResponse placeDescriptionResponse =
                                        placeDescriptionService.responseBodyToModel(responseBody);

                                //Objects.requireNonNull(placeDescriptionResponse.getName(), "Name is null");
                                //Objects.requireNonNull(placeDescriptionResponse.getPoint(), "Point is null");
                                //Objects.requireNonNull(placeDescriptionResponse.getInfo(), "Info is null");
                                //Objects.requireNonNull(placeDescriptionResponse.getInfo().getDescr(), "Descr is null");
                                //System.out.println(placeDescriptionResponse.toString());
                                future.complete(mapPlaceDescriptionResponseToPlace(placeDescriptionResponse));
                            } catch (NullPointerException e) {
                                future.completeExceptionally(e);
                            }
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
        }

        CompletableFuture<Void> allOf = CompletableFuture.allOf(futures.toArray(new CompletableFuture[0]));

        return allOf.thenApply(v ->
                futures.stream()
                        .map(CompletableFuture::join)
                        .toArray(Place[]::new)
        );
    }

    private Place mapPlaceDescriptionResponseToPlace(PlaceDescriptionResponse placeDescriptionResponse) {
        Place place = new Place();
        place.setName(placeDescriptionResponse.getName());
        place.setLat(placeDescriptionResponse.getPoint().getLat());
        place.setLng(placeDescriptionResponse.getPoint().getLon());
        place.setDescription(placeDescriptionResponse.getInfo().getDescr());

        return place;
    }
}
