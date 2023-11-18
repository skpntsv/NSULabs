package ru.nsu.skopintsev.api.services;

import com.google.gson.Gson;
import okhttp3.Request;
import okhttp3.ResponseBody;
import ru.nsu.skopintsev.api.responses.PlaceDescriptionResponse;

import java.io.IOException;

public class PlaceDescriptionService {
    private static final String API_URL = "https://api.opentripmap.com/0.1/ru/places/xid/";

    public Request getRequest(String xid) {
        String apiKey = "YOUR_API_KEY";
        String apiUrl = API_URL + xid + "?apikey=" + apiKey;

        Request request = new Request.Builder()
                .url(apiUrl)
                .get()
                .build();

        return request;
    }

    public PlaceDescriptionResponse responseBodyToModel(ResponseBody responseBody) throws IOException {
        String json = responseBody.string();

        Gson gson = new Gson();

        return gson.fromJson(json, PlaceDescriptionResponse.class);
    }
}
