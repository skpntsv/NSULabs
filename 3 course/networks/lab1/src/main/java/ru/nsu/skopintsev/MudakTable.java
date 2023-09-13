package ru.nsu.skopintsev;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class MudakTable {
    private Map<String, Long> ipAddressTimestampMap;

    public MudakTable() {
        this.ipAddressTimestampMap = new HashMap<>();
    }

    public void updateEntry(String ipAddress) {
        ipAddressTimestampMap.put(ipAddress, System.currentTimeMillis());
    }

    public void removeEntry(String ipAddress) {
        ipAddressTimestampMap.remove(ipAddress);
    }

    public void printTable() {
        System.out.println("M.U.D.A.K. Table:");
        long currentTime = System.currentTimeMillis();

        for (Map.Entry<String, Long> entry : ipAddressTimestampMap.entrySet()) {
            String ipAddress = entry.getKey();
            long lastUpdateTime = entry.getValue();
            long timeDifference = currentTime - lastUpdateTime;

            String timeAgo = formatTimeAgo(timeDifference);
            System.out.println("IP Address: " + ipAddress + ", Last Update: " + timeAgo);
        }
    }

    private String formatTimeAgo(long timeDifference) {
        long seconds = TimeUnit.MILLISECONDS.toSeconds(timeDifference);
        return seconds + " seconds ago";
    }
}
