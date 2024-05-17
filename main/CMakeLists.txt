idf_component_register(
    SRCS
    "main.cpp"
    "NetworkManager/NetworkManager.hpp"   "NetworkManager/NetworkManager.cpp" 
    "Modbus/MbClient.hpp"                 "Modbus/MbClient.cpp"
    "Modbus/MbManager.hpp"                "Modbus/MbManager.cpp"
    "Mqtt/MqttClient.hpp"                 "Mqtt/MqttClient.cpp"
    INCLUDE_DIRS "."
)
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)

