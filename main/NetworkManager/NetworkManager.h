#pragma once

typedef const char* esp_event_base_t;
typedef __INT32_TYPE__ int32_t;

extern int CONNECTED;

void WiFiEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id,void* event_data);
void WiFiConnection();

void NetworkManagerInit();

static void TcpServerTask(void* pvParameters);

void NetworkManagerRun();
