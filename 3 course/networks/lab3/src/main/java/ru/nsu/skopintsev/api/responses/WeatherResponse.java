package ru.nsu.skopintsev.api.responses;

import com.google.gson.annotations.SerializedName;
import lombok.Data;

import java.util.List;

@Data
public class WeatherResponse {
    private Coord coord;
    private List<Weather> weather;
    private String base;
    private Main main;
    private Long visibility;
    private Wind wind;
    private Clouds clouds;
    private Rain rain;
    private Snow snow;
    private Long dt;
    private Sys sys;
    private Long timezone;
    private Long id;
    private String name;
    private Long cod;

    @Data
    public static class Coord {

        private Double lat;
        private Double lon;
    }

    @Data
    public static class Weather {

        private Long id;
        private String main;
        private String description;
        private String icon;
    }

    @Data
    public static class Main {

        private Double temp;
        private Double feels_like;
        private Double temp_min;
        private Double temp_max;
        private Long pressure;
        private Long humidity;
        private Long sea_level;
        private Long grnd_level;
    }

    @Data
    public static class Wind {

        private Double speed;
        private Long deg;
        private Double gust;
    }

    @Data
    public static class Clouds {

        private Long all;
    }

    @Data
    public static class Rain {

        @SerializedName("1h")
        private Double _1h;


        @SerializedName("3h")
        private Double _3h;
    }

    @Data
    public static class Snow {

        @SerializedName("1h")
        private Double _1h;


        @SerializedName("3h")
        private Double _3h;
    }

    @Data
    public static class Sys {

        private Long type;
        private Long id;
        private String country;
        private Long sunrise;
        private Long sunset;
    }
}
