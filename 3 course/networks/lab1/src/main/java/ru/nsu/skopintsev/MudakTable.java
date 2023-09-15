package ru.nsu.skopintsev;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class MudakTable {
    private final Map<String, Long> users;

    public MudakTable() {
        this.users = new HashMap<>();
    }

    public void updateEntry(String string) {
        users.put(string, System.currentTimeMillis());
    }

    public void removeEntry(String string) {
        users.remove(string);
    }

    public void killZombie() {
        long currentTime = System.currentTimeMillis();
        users.entrySet().removeIf(entry -> {
            long lastUpdateTime = entry.getValue();
            long timeDifference = currentTime - lastUpdateTime;
            return TimeUnit.MILLISECONDS.toSeconds(timeDifference) > 20;
        });
    }

    public void printTable() {
        System.out.println("M.U.D.A.K. Table:");
        long currentTime = System.currentTimeMillis();

        for (Map.Entry<String, Long> entry : users.entrySet()) {
            String string = entry.getKey();
            long lastUpdateTime = entry.getValue();
            long timeDifference = currentTime - lastUpdateTime;

            String timeAgo = formatTimeAgo(timeDifference);
            System.out.println("IP Address: " + string + ", Last Update: " + timeAgo);
        }
    }

    private String formatTimeAgo(long timeDifference) {
        long seconds = TimeUnit.MILLISECONDS.toSeconds(timeDifference);
        return seconds + " seconds ago";
    }
}
