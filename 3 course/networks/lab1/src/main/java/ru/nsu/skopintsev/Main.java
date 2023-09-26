/*Обнаружение копий себя в локальной сети */
/*
Разработать приложение, обнаруживающее копии себя в локальной сети с помощью обмена
multicast UDP сообщениями. Приложение должно отслеживать моменты появления и
исчезновения других копий себя в локальной сети и при изменениях выводить список
IP адресов "живых" копий.

Адрес multicast-группы необходимо передавать параметром в приложение.
Приложение должно поддерживать работу как в IPv4 так и в IPv6 сетях,
выбирая протокол автоматически в зависимости от переданного адреса группы.

*/

package ru.nsu.skopintsev;

import ru.nsu.skopintsev.utils.ConfigParser;

public class Main {
    public static void main(String[] args) {
        String multicastGroupAddress = ConfigParser.IP;
        int multicastPort = ConfigParser.PORT;

        MulticastDiscovery discovery = new MulticastDiscovery(multicastGroupAddress, multicastPort);
        discovery.start();
    }
}