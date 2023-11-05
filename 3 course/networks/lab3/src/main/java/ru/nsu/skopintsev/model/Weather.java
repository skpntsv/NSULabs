package ru.nsu.skopintsev.model;

import lombok.Data;

@Data
public class Weather {
    private Long temperature;
    private Long feelsLikeTemperature;
    private Long windSpeed;
    private String windDirection;
    private Long pressure;
    private Long humidity;
    private String desc;
}
