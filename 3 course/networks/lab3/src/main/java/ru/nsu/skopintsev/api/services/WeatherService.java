package ru.nsu.skopintsev.api.services;

import com.google.gson.Gson;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;
import org.jetbrains.annotations.NotNull;
import ru.nsu.skopintsev.api.responses.LocationResponse;
import ru.nsu.skopintsev.api.responses.WeatherResponse;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public class WeatherService {
    private static final String API_URL = "https://api.openweathermap.org/data/2.5/weather";
    private final OkHttpClient httpClient;
    private final Gson gson;

    public WeatherService() {
        httpClient = new OkHttpClient();
        gson = new Gson();
    }

    public CompletableFuture<WeatherResponse> getWeatherByCoordinates(double lat, double lng) {
        CompletableFuture<WeatherResponse> future = new CompletableFuture<>();

        String apiKey = "2f8c8261-8ee6-4e53-a7f9-fb7a14957d39";
        String apiUrl = API_URL + "?lat=" + lat + "&lon=" + lng +"&key=" + apiKey;

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
                        WeatherResponse weatherResponse = gson.fromJson(json, WeatherResponse.class);
                        future.complete(weatherResponse);
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
