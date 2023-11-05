package ru.nsu.skopintsev.api.services;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;
import com.google.gson.Gson;
import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.api.responses.LocationResponse;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public class LocationService {
    private static final String API_URL = "https://graphhopper.com/api/1/geocode";
    private final OkHttpClient httpClient;
    private final Gson gson;

    public LocationService() {
        httpClient = new OkHttpClient();
        gson = new Gson();
    }

    public CompletableFuture<LocationResponse.Location[]> searchLocations(String query) {
        CompletableFuture<LocationResponse.Location[]> future = new CompletableFuture<>();

        
        String apiUrl = API_URL + "?q=" + query + "&key=" + apiKey;

        Request request = new Request.Builder()
                .url(apiUrl)
                .get()
                .build();

        httpClient.newCall(request).enqueue(new okhttp3.Callback() {
            @Override
            public void onResponse(@NotNull okhttp3.Call call, @NotNull Response response) throws IOException {
                if (response.isSuccessful()) {
                    ResponseBody responseBody = response.body();
                    if (responseBody != null) {
                        String json = responseBody.string();
                        LocationResponse locationResponse = gson.fromJson(json, LocationResponse.class);
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
