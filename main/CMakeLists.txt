idf_component_register(
    SRCS
    "main.c"
    "NetworkManager/NetworkManager.c" "NetworkManager/NetworkManager.h"
    "NetworkManager/Tests.c" "Networkmanager/Tests.h"
    "Modbus/MbClient.h" "Modbus/Modbus.h" "Modbus/Modbus.cpp" "Modbus/MbClient.cpp"
    INCLUDE_DIRS "."
)
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)

