package ru.nsu.skopintsev.model;

import lombok.Data;

@Data
public class Weather {
    private double temperature;
    private double feelsLikeTemperature;
    private double windSpeed;
    private double windDirection;
    private Long pressure;
    private Long humidity;
    private String desc;

    @Override
    public String toString() {
        return "Temperature: " + temperature + " °C\n" +
                "Feels Like Temperature: " + feelsLikeTemperature + " °C\n" +
                "Wind Speed: " + windSpeed + " m/s\n" +
                "Wind Direction: " + windDirection + "°\n" +
                "Pressure: " + pressure + " hPa\n" +
                "Humidity: " + humidity + "%\n" +
                "Description: " + desc + "\n";
    }
}
