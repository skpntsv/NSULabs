package ru.nsu.skopintsev;

import ru.nsu.skopintsev.controller.*;
import ru.nsu.skopintsev.model.*;

import java.util.Scanner;
import java.util.concurrent.CompletableFuture;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.print("Введите название локации: ");
        String locationName = scanner.nextLine();

        LocationController locationController = new LocationController();
        CompletableFuture<Location[]> locationFuture = locationController.searchLocations(locationName);

        locationFuture.thenAccept(locations -> {
            try {
                if (locations.length > 0) {
                    System.out.println("Выберите локацию:");
                    for (int i = 0; i < locations.length; i++) {
                        System.out.println(i + 1 + ". " + locations[i]);
                    }

                    System.out.print("Введите номер выбранной локации: ");
                    int selectedLocationIndex = Integer.parseInt(scanner.nextLine()) - 1;
                    System.out.println();
                    Location selectedLocation = locations[selectedLocationIndex];

                    WeatherController weatherController = new WeatherController();
                    CompletableFuture<Weather> weatherFuture = weatherController.getWeatherByCoords(
                            selectedLocation.getLat(), selectedLocation.getLng());

                    PlacesController placesController = new PlacesController();

                    CompletableFuture<String[]> placesFuture = placesController.getInterestingPlaces(
                            selectedLocation.getLat(), selectedLocation.getLng());

                    CompletableFuture<Place[]> placesWithDescFuture = placesFuture.thenCompose(xids -> {
                        PlaceDescriptionController placeDescriptionController = new PlaceDescriptionController();
                        return placeDescriptionController.getPlacesDescriptions(xids);
                    });

                    CompletableFuture<Void> combinedFuture = CompletableFuture.allOf(weatherFuture, placesWithDescFuture);

                    combinedFuture.thenAccept(ignored -> {
                        try {
                            Result result = new Result(selectedLocation, weatherFuture.join(), placesWithDescFuture.join());

                            showFinalResults(result);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    });
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
    }


    private static void showFinalResults(Result result) {
        System.out.println("Погода в " + result.getLocation().getName() + ":");
        System.out.println(result.getWeather());

        for (Place place : result.getPlaces()) {
            System.out.println(place);
            System.out.println();
        }
    }
}
