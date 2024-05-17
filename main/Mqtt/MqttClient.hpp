#include <string>

#include "mqtt_client.h"

class MqttClient {
public:
    static MqttClient* GetInstance();

    void Publish(std::string msg, std::string topic);

private:
    MqttClient();
    
    MqttClient(MqttClient&) = delete;
    void operator = (const MqttClient&) = delete;

    void Start();
private:
    esp_mqtt_client_handle_t m_client;

    static MqttClient* s_instance;
};