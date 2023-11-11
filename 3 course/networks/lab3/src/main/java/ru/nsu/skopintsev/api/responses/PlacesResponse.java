package ru.nsu.skopintsev.api.responses;

import com.google.gson.annotations.SerializedName;
import lombok.Data;

@Data
public class PlacesResponse {
    private Place[] places;

    public PlacesResponse(Place[] places) {
        this.places = places;
    }

    @Data
    public static class Place {
        private String name;
        private String osm;
        private String xid;
        private String wikidata;
        private String kind;
        private Point point;

        @Data
        public static class Point {
            @SerializedName("lon")
            private double lng;

            private double lat;
        }
    }
}
