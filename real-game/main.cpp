#include "mbed.h"
#include <cstdio>

// sensor module header
#include "Sensor/my_sensor.h"

// wifi module header
// #include "Wifi/my_wifi.h"

// BLE module header
// #include "BLE/my_ble.h"



// handle console ouput
static BufferedSerial serial_port(USBTX, USBRX);
FileHandle *mbed::mbed_override_console(int fd)
{
    return &serial_port; 
}

// I/O Devices


// EventQueue
EventQueue event_queue(32 * EVENTS_EVENT_SIZE);
Thread t;


int main() 
{
    // wifi variables
    int count = 0;

    // printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    // int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2); 
    
    // if (ret != 0) {
    //     printf("\nConnection error\n");
    //     return -1; 
    // }

    // printf("Success\n\n");
    // printf("MAC: %s\n", wifi.get_mac_address()); 
    // printf("IP: %s\n", wifi.get_ip_address()); 
    // printf("Netmask: %s\n", wifi.get_netmask()); 
    // printf("Gateway: %s\n", wifi.get_gateway()); 
    // printf("RSSI: %d\n\n", wifi.get_rssi());
    

    // // http_demo(&wifi);
    // send_sensor_data(&wifi);
    // printf("sensor data complete");
    // wifi.disconnect();
    // printf("\nDone\n"); 


    // BLE &ble = BLE::Instance();
    // events::EventQueue event_queue;
    // LocationService demo_service;

    // /* this process will handle basic ble setup and advertising for us */
    // GattServerProcess ble_process(event_queue, ble);

    // /* once it's done it will let us continue with our demo */
    // ble_process.on_init(callback(&demo_service, &LocationService::start));

    // ble_process.start();
    DataSensor* sensor = new DataSensor(event_queue);

    t.start(callback(&event_queue, &EventQueue::dispatch_forever));

    sensor->start();

    return 0;
}