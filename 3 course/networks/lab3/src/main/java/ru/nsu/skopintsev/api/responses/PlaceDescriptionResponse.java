package ru.nsu.skopintsev.api.responses;

import lombok.Data;

@Data
public class PlaceDescriptionResponse {
    private String kinds;
    private Sources sources;
    private Bbox bbox;
    private Point point;
    private String osm;
    private String otm;
    private String xid;
    private String name;
    private String wikipedia;
    private String image;
    private String wikidata;
    private String rate;
    private Info info;

    @Data
    public static class Sources {
        private String geometry;
        private String[] attributes;
    }

    @Data
    public static class Bbox {
        private double lat_max;
        private double lat_min;
        private double lon_max;
        private double lon_min;
    }

    @Data
    public static class Point {
        private double lon;
        private double lat;
    }

    @Data
    public static class Info {
        private String descr;
        private String image;
        private int img_width;
        private String src;
        private int src_id;
        private int img_height;
    }
}
