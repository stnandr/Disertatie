idf_component_register(
    SRCS
    "main.cpp"
    "NetworkManager/NetworkManager.h"   "NetworkManager/NetworkManager.cpp" 
    "Modbus/MbClient.h"                 "Modbus/MbClient.cpp"
    "Modbus/MbManager.h"                "Modbus/MbManager.cpp"
    INCLUDE_DIRS "."
)
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)

