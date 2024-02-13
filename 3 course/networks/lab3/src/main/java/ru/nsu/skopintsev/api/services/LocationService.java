package ru.nsu.skopintsev.api.services;

import com.google.gson.Gson;
import okhttp3.Request;
import okhttp3.ResponseBody;
import ru.nsu.skopintsev.api.responses.LocationResponse;

import java.io.IOException;

public class LocationService {
    private static final String API_URL = "https://graphhopper.com/api/1/geocode";

    public Request getRequest(String query) {
        String apiKey = "YOUR_API_KEY";
        String apiUrl = API_URL + "?q=" + query + "&key=" + apiKey;

        Request request = new Request.Builder()
                .url(apiUrl)
                .get()
                .build();

        return request;
    }

    public LocationResponse responseBodyToLocationResponse(ResponseBody responseBody) throws IOException {
        String json = responseBody.string();

        Gson gson = new Gson();

        return gson.fromJson(json, LocationResponse.class);
    }
}
