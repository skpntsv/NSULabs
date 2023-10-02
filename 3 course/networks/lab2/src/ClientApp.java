import client.Client;

public class ClientApp {
    public static void main(String[] args) {
        String address = "127.0.0.1";
        int port = 1234;
        //String filePath = "/home/skpntsv/git/NSULabs/3 course/networks/lab2/src/rggdfsghghhsdhs.txt";
        String filePath = "/home/skpntsv/downloads/not-rikroll.mp4";

        new Client(address, port, filePath);
    }
}
