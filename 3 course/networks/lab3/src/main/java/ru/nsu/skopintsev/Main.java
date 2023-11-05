package ru.nsu.skopintsev;

import ru.nsu.skopintsev.api.responses.LocationResponse;
import ru.nsu.skopintsev.api.responses.WeatherResponse;
import ru.nsu.skopintsev.controller.*;
import ru.nsu.skopintsev.model.*;

import java.util.Scanner;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.print("Введите название локации: ");
        //String locationName = scanner.nextLine();
        String locationName = "Новосибирск";
        // Асинхронно получаем локации
        LocationController locationController = new LocationController();
        CompletableFuture<LocationResponse.Location[]> locationFuture = locationController.searchLocations(locationName);

        locationFuture.thenAccept(locations -> {
            if (locations.length > 0) {
                System.out.println("Выберите локацию:");
                for (int i = 0; i < locations.length; i++) {
                    System.out.println(i + 1 + ". " + locations[i].getName());
                }

                System.out.print("Введите номер выбранной локации: ");
                //int selectedLocationIndex = Integer.parseInt(scanner.nextLine()) - 1;
                int selectedLocationIndex = 0;
                LocationResponse.Location selectedLocation = locations[selectedLocationIndex];

                WeatherController weatherController = new WeatherController();
                CompletableFuture<WeatherResponse> weatherFuture = weatherController.
                        getWeatherByCoordinates(selectedLocation.getPoint().getLat(), selectedLocation.getPoint().getLng());

                weatherFuture.thenAccept(weather -> {
                    //System.out.println("Погода в " + selectedLocation.getName() + ": " + weather.getTemperature() + "°C");
                    System.out.println(weather.toString());
                    //CompletableFuture<InterestingPlaces> placesFuture = PlacesAPI.getInterestingPlaces(selectedLocation.getCoordinates());

//                    placesFuture.thenAccept(places -> {
//                        System.out.println("Интересные места в " + selectedLocation.getName() + ":");
//                        for (InterestingPlace place : places.getPlaces()) {
//                            CompletableFuture<String> descriptionFuture = PlacesAPI.getPlaceDescription(place.getId());
//                            descriptionFuture.thenAccept(description -> {
//                                System.out.println(place.getName() + ": " + description);
//                            });
//                        }
//                    });
                });
            } else {
                System.out.println("Локации не найдены");
            }
        });

        try {
            CompletableFuture<Void> allOf = CompletableFuture.allOf(locationFuture/*, weatherFuture, placesFuture*/);
            allOf.get(); // Блокировка ожидания завершения всех CompletableFuture
        } catch (InterruptedException | ExecutionException e) {
            e.printStackTrace();
        }
    }
}
