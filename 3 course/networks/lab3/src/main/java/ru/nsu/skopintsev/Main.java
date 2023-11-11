package ru.nsu.skopintsev;

import ru.nsu.skopintsev.api.responses.PlacesResponse;
import ru.nsu.skopintsev.controller.LocationController;
import ru.nsu.skopintsev.controller.PlacesController;
import ru.nsu.skopintsev.controller.WeatherController;
import ru.nsu.skopintsev.model.Location;
import ru.nsu.skopintsev.model.Weather;

import java.util.Scanner;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.print("Введите название локации: ");
        String locationName = scanner.nextLine();

        LocationController locationController = new LocationController();
        CompletableFuture<Location[]> locationFuture = locationController.searchLocations(locationName);

        locationFuture.thenAccept(locations -> {
            if (locations.length > 0) {
                System.out.println("Выберите локацию:");
                for (int i = 0; i < locations.length; i++) {
                    System.out.println(i + 1 + ". " + locations[i].toString());
                }

                System.out.print("Введите номер выбранной локации: ");
                int selectedLocationIndex = Integer.parseInt(scanner.nextLine()) - 1;
                Location selectedLocation = locations[selectedLocationIndex];

                WeatherController weatherController = new WeatherController();
                CompletableFuture<Weather> weatherFuture = weatherController.
                        getWeatherByCoords(selectedLocation.getLat(), selectedLocation.getLng());

                weatherFuture.thenAccept(weather -> {
                    System.out.println("Информация о погоде:\n" + weather.toString());

                    PlacesController placesController = new PlacesController();
                    CompletableFuture<PlacesResponse> placesFuture = placesController.getInterestingPlaces(selectedLocation.getLat(), selectedLocation.getLng());

                    placesFuture.thenAccept(places -> {
                        System.out.println("Интересные места в " + selectedLocation.getName() + ":");
                        for (PlacesResponse.Place place : places.getPlaces()) {
                            System.out.println(place.toString());
                        }
                    });
                });
            } else {
                System.out.println("Локации не найдены");
            }
        });

        try {
            CompletableFuture<Void> allOf = CompletableFuture.allOf(locationFuture);
            allOf.get();
        } catch (InterruptedException | ExecutionException e) {
            e.printStackTrace();
        }
    }
}
