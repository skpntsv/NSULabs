package ru.nsu.skopintsev.api.services;

import com.google.gson.Gson;
import okhttp3.Request;
import okhttp3.ResponseBody;
import ru.nsu.skopintsev.api.responses.PlacesResponse;

import java.io.IOException;

public class PlacesService {
    private static final String API_URL = "https://api.opentripmap.com/0.1/ru/places/radius";

    public Request getRequest(double lat, double lng, int radius) {

        String apiUrl = API_URL + "?radius=" + radius + "&lat=" + lat + "&lon=" + lng +
                "&format=json" + "&apikey=" + apiKey;

        Request request = new Request.Builder()
                .url(apiUrl)
                .get()
                .build();

        return request;
    }

    public PlacesResponse responseBodyToModel(ResponseBody responseBody) throws IOException {
        String json = responseBody.string();

        Gson gson = new Gson();
        PlacesResponse.Place[] places = gson.fromJson(json, PlacesResponse.Place[].class);

        return new PlacesResponse(places);
    }
}
