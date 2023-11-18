package ru.nsu.skopintsev.api.services;

import com.google.gson.Gson;
import okhttp3.Request;
import okhttp3.ResponseBody;
import ru.nsu.skopintsev.api.responses.WeatherResponse;

import java.io.IOException;

public class WeatherService {
    private static final String API_URL = "https://api.openweathermap.org/data/2.5/weather";

    public Request getRequest(double lat, double lng) {
        String apiKey = "YOUR_API_KEY";
        String apiUrl = API_URL + "?units=metric" + "&lat=" + lat + "&lon=" + lng +"&appid=" + apiKey;

        Request request = new Request.Builder()
                .url(apiUrl)
                .get()
                .build();

        return request;
    }

    public WeatherResponse responseBodyToModel(ResponseBody responseBody) throws IOException {
        String json = responseBody.string();

        Gson gson = new Gson();

        return gson.fromJson(json, WeatherResponse.class);
    }
}
