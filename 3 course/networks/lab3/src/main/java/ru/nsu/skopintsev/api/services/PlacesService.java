package ru.nsu.skopintsev.api.services;

import ru.nsu.skopintsev.model.Place;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;
import com.google.gson.Gson;
import org.jetbrains.annotations.NotNull;
import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public class PlacesService {
    private final OkHttpClient httpClient;
    private final Gson gson;

    public PlacesService() {
        httpClient = new OkHttpClient();
        gson = new Gson();
    }

    public CompletableFuture<Place[]> getInterestingPlaces(double lat, double lng) {
        CompletableFuture<Place[]> future = new CompletableFuture<>();

        String apiKey = "YOUR_API_KEY";

        String apiUrl = "https://api.opentripmap.io/0.1/en/places/radius?radius=1000&limit=10" +
                "&lon=" + lat +
                "&lat=" + lng +
                "&apikey=" + apiKey;

        Request request = new Request.Builder()
                .url(apiUrl)
                .build();

        httpClient.newCall(request).enqueue(new okhttp3.Callback() {
            @Override
            public void onResponse(@NotNull okhttp3.Call call, @NotNull Response response) throws IOException {
                if (response.isSuccessful()) {
                    ResponseBody responseBody = response.body();
                    if (responseBody != null) {
                        String json = responseBody.string();
                        Place[] places = gson.fromJson(json, Place[].class);
                        future.complete(places);
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
