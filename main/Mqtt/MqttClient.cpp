#include "MqttClient.hpp"

#include "esp_log.h"

#define TAG "MQTT"

namespace
{
    const     std::string BROKER_URL = "mqtt://192.168.0.17";
    const     std::string BROKER_USR = "stanandrei";
    const     std::string BROKER_PSW = "123";
    constexpr int16_t     QOS_LEVEL  = 1;
    constexpr int16_t     RETAIN     = 0;
}

MqttClient* MqttClient::GetInstance()
{
    if (!s_instance)
        s_instance = new MqttClient();
    
    return s_instance;
}

void MqttClient::Publish(std::string msg, std::string topic)
{
    int msgId = esp_mqtt_client_publish(m_client, topic.c_str(), msg.c_str(), msg.length(), QOS_LEVEL, RETAIN);
    ESP_LOGI(TAG, "Pusblished message: %d", msgId);
}

static void EventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {}

MqttClient::MqttClient()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .uri = BROKER_URL.c_str(),
            },          
        },
        .credentials = {
            .username = BROKER_USR.c_str(),
            .authentication = {
                .password = BROKER_PSW.c_str(),
            },
        },
    };

    m_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(m_client, esp_mqtt_event_id_t::MQTT_EVENT_ANY, EventHandler, NULL);

    /*Let's enqueue a few messages to the outbox to see the allocations*/
    //int msg_id;
    //msg_id = esp_mqtt_client_enqueue(client, "/topic/qos1", "data_3", 0, 1, 0, true);
    //ESP_LOGI(TAG, "Enqueued msg_id=%d", msg_id);
    //msg_id = esp_mqtt_client_enqueue(client, "/topic/qos2", "QoS2 message", 0, 2, 0, true);
    //ESP_LOGI(TAG, "Enqueued msg_id=%d", msg_id);

    /* Now we start the client and it's possible to see the memory usage for the operations in the outbox. */
    esp_mqtt_client_start(m_client);
}

MqttClient* MqttClient::s_instance = nullptr;