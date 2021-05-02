/* WiFi Example
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
// wifi module header
#include "./wifi.h"
#include "../Sensor/sensor.h"

ISM43362Interface wifi(false);

#if MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1
#include "SpwfSAInterface.h"
SpwfSAInterface wifi(MBED_CONF_APP_WIFI_TX, MBED_CONF_APP_WIFI_RX);
#endif // MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1

const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

void print_wifi_info(ISM43362Interface &wifi) {
    printf("MAC: %s\n", wifi.get_mac_address()); 
    printf("IP: %s\n", wifi.get_ip_address()); 
    printf("Netmask: %s\n", wifi.get_netmask()); 
    printf("Gateway: %s\n", wifi.get_gateway()); 
    printf("RSSI: %d\n\n", wifi.get_rssi());
}


/*
 * Example:
   count = scan_demo(&wifi); 
   if (count == 0) {
        printf("No WIFI APNs found - can't continue further.\n");
        return -1; 
    }
 */
int scan_devices(WiFiInterface *wifi, int limit_count = 15)
{
    WiFiAccessPoint *ap;

    printf("Scan:\n");

    int count = wifi->scan(NULL,0);
    printf("%d networks available.\n", count);

    /* Limit number of network arbitrary to 15 */
    count = count < limit_count ? count : limit_count;

    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);

    for (int i = 0; i < count; i++)
    {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }

    delete[] ap;
    return count;
}

void send_sensor_data(NetworkInterface *net)
{
    TCPSocket socket;
    nsapi_error_t response;

    printf("Sending data to host computer...\n");

    // Show the network address
    SocketAddress a;
    const char * IP_ADDRESS = "192.168.50.252";
    // Open a socket on the network interface, and create a TCP connection to //www.arm.com
    socket.open(net); 
    if(!a.set_ip_address(IP_ADDRESS)) {
        printf("Set IP address failed");
        return ;
    }

    a.set_port(30007);
    
    response = socket.connect(a);

    if(0 != response) {
        printf("Error connecting: %d\n", response);
        socket.close();
        return;
    }

    float sensor_value = 0;
    int16_t pDataXYZ[3] = {0};
    float pGyroDataXYZ[3] = {0};

    printf("Start sensor init\n");

    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();

    BSP_MAGNETO_Init();
    BSP_GYRO_Init();
    BSP_ACCELERO_Init();
    int count = 0;

    while(1) {
        count++;
        printf("\nSending data to the server ........\n");
        char buffer[1024] = {0}; 

        // Gyro
        BSP_GYRO_GetXYZ(pGyroDataXYZ);

        // acceleration
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        int len = sprintf(buffer,"{\"a_x\":%d,\"a_y\":%d,\"a_z\":%d,\"g_x\":%.2f,\"g_y\":%.2f,\"g_z\":%.2f,\"s\":%d}",
        pDataXYZ[0], pDataXYZ[1], pDataXYZ[0], pGyroDataXYZ[0], pGyroDataXYZ[1], pGyroDataXYZ[2], count);

        response = socket.send(buffer,len); 
        if (0 >= response){
            printf("Error seding: %d\n", response); 
        }

        ThisThread::sleep_for(100);
    }

    socket.close();
}
