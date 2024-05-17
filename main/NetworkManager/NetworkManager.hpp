#pragma once

typedef const char* esp_event_base_t;
typedef __INT32_TYPE__ int32_t;

extern int CONNECTED;

class NetworkManager
{
public:
    static void Init();

private:
    struct NetworkManagerData
    {
        const char* ssid;
        const char* pass;
        int retry_num;
    };

    static void WiFiEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id,void* event_data);
    static void WiFiConnection();
};