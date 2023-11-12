package ru.nsu.skopintsev;

import ru.nsu.skopintsev.controller.*;
import ru.nsu.skopintsev.model.*;

import java.util.Arrays;
import java.util.Scanner;
import java.util.concurrent.CompletableFuture;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.print("Введите название локации: ");
        String locationName = scanner.nextLine();

        LocationController locationController = new LocationController();
        CompletableFuture<Location[]> locationFuture = locationController.searchLocations(locationName);

        locationFuture.thenCompose(locations -> {
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

                return weatherFuture.thenCombine(placesFuture, (weather, xids) -> {
                    System.out.println("Погода в " + selectedLocation.getName() + ":");
                    System.out.println(weather);

                    System.out.println("\nИнтересные места в " + selectedLocation.getName() + ":");
                    CompletableFuture<Void> allDescriptions = CompletableFuture.allOf(
                            Arrays.stream(xids).map(xid -> {
                                PlaceDescriptionController placeDescriptionController = new PlaceDescriptionController();
                                CompletableFuture<Place> placeFuture = placeDescriptionController.getPlaceDescription(xid);

                                return placeFuture.thenAccept(place -> {
                                    if (place.getName() != null && place.getDescription() != null) {
                                        System.out.println(place);
                                        System.out.println();
                                    }
                                });
                            }).toArray(CompletableFuture[]::new)
                    );

                    return allDescriptions.thenApply(v -> null);
                });
            } else {
                System.out.println("Локации не найдены");
                return CompletableFuture.completedFuture(null);
            }
        }).join();
    }
}
