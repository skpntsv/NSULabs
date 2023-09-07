#include <iostream>
#include <libusb-1.0/libusb.h>
#include <cstdio>

using namespace std;

void printdev(libusb_device *dev);

int main() {

	libusb_device** devs; // указатель на указатель на устройство,
	// используется для получения списка устройств

	libusb_context* ctx = NULL; // контекст сессии libusb
	int r;						// для возвращаемых значений
	ssize_t cnt;				// число найденных USB-устройств
	ssize_t i;					// индексная переменная цикла перебора всех устройств

	// инициализировать библиотеку libusb, открыть сессию работы с libusb
	r = libusb_init(&ctx);
    
    if (r < 0) {
	    fprintf(stderr, "Ошибка: инициализация не выполнена, код: %d.\n", r);
	    return 1;
    }

	// получить список всех найденных USB- устройств
	cnt = libusb_get_device_list(ctx, &devs);

    if (cnt < 0) {
	    fprintf(stderr, "Ошибка: список USB устройств не получен.\n");
	    return 1;
    }

    printf("Найдено устройств: %ld\n", cnt);
	printf("==============================""=============================\n");
    for (i = 0; i < cnt; i++) {
	    printdev(devs[i]);
		printf("+--+--+----+----+---+--+--+--+""--+--+----------------------\n");
    }
	printf("==============================""=============================\n");


	// освободить память, выделенную функцией получения списка устройств
	libusb_free_device_list(devs, 1);

	libusb_exit(ctx); // завершить работу с библиотекой libusb,
	// закрыть сессию работы с libusb

    return 0;
}


void printdev(libusb_device *dev) {
	libusb_device_descriptor desc; // дескриптор устройства

    int r = libusb_get_device_descriptor(dev, &desc); // получить дескриптор устройства
    if (r < 0) {
	    fprintf(stderr, "Ошибка: дескриптор устройства не получен, код: %d.\n", r);
	    return;
    }

    printf("Kласс устройства: %d\n", (int)desc.bDeviceClass);


	unsigned char str[256];

	libusb_device_handle* handle = NULL;

    libusb_open(dev, &handle);
    if (handle) {
	    if (desc.iSerialNumber) {
	        r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, str, sizeof(str));
			printf("Cерийный номер: ");
	        if (r > 0) 
				printf("%s\n", str); 
            else 
				printf("Нет данных\n");
	    }
	    if (desc.iManufacturer) {
	        r = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, str, sizeof(str));
			printf("Производитель: ");
	        if (r > 0) 
				printf("%s\n", str);
	        else  
				printf("Нет данных\n");
	    }
	    if (desc.iProduct) {
	        r = libusb_get_string_descriptor_ascii(handle, desc.iProduct, str, sizeof(str));
			printf("Имя устройства: ");
	        if (r > 0) 
				printf("%s", str);
	        else 
				printf("Нет данных\n");
	    }
    } 
    else  
		printf("\t\tДоступ к данным закрыт");
    printf("\n");

	libusb_close(handle);
}










