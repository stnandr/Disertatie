#include "NetworkManager.h"
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"  //for delay,mutexs,semphrs rtos operations
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"         //esp_init funtions esp_err_t 
#include "esp_wifi.h"           //esp_wifi_init functions and wifi operations
#include "esp_log.h"            //for showing logs
#include "esp_event.h"          //for wifi event
#include "nvs_flash.h"          //non volatile storage
#include "lwip/err.h"           //light weight ip packets error handling
#include "lwip/sys.h"           //system applications for light weight ip apps
#include "lwip/sockets.h"
#include <lwip/netdb.h>

#define PORT 502
int CONNECTED = 0;

static const char* TAG = "TCP/IP socket";

const char* ssid = "Orange-E1C6";
const char* pass = "QsZRX7EcEdzKt4XcdN";
int retryNum=0;

struct NetworkManagerData {
    const char* ssid;
    const char* pass;
    int retry_num;
};

void WiFiEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    //
    // TODO: Change this to a switch case
    if(event_id == WIFI_EVENT_STA_START)
        printf("Device connecting to WiFi...\n");
    else if (event_id == WIFI_EVENT_STA_CONNECTED)
        printf("Device CONNECTED to WiFi\n");
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        printf("Device LOST connection\n");

        if (retryNum < 10)
        {
            esp_wifi_connect();
            retryNum++;
            printf("Retrying to Connect...\n");
        }
    }
    else if (event_id == IP_EVENT_STA_GOT_IP)
    {
        printf("Device got IP...\n\n");
        CONNECTED = 1;
    }
}

void WiFiConnection()
{
    esp_netif_init();

    //
    // Create default configuration
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);

    //
    // Hook event handler
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, WiFiEventHandler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, WiFiEventHandler, NULL);
    
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = "",
        }
    };

    strcpy((char*)wifi_configuration.sta.ssid, ssid);
    strcpy((char*)wifi_configuration.sta.password, pass);    
    
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

    //
    // Start wifi
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_connect();
    
    printf("wifi_init_softap finished. SSID:%s password:%s",ssid,pass);
}

void NetworkManagerInit()
{
    nvs_flash_init();
    WiFiConnection();
}
