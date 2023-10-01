import client.Client;

public class ClientApp {
    public static void main(String[] args) {
        String address = "127.0.0.1";
        int port = 1234;
        String filePath = "src/test.txt";

        new Client(address, port, filePath);
    }
}
