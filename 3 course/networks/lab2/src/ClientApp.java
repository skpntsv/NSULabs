import client.Client;

public class ClientApp {
    public static void main(String[] args) {
        String address = "192.168.121.11";
        int port = 34567;
        String filePath = "/home/skpntsv/downloads/video.mp4";
        //String filePath = "/home/skpntsv/downloads/not-rikroll.mp4";

        new Client(address, port, filePath);
    }
}
