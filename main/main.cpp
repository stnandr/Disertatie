#include "NetworkManager/NetworkManager.hpp"
#include "Modbus/MbManager.hpp"
#include "Modbus/MbClient.hpp"

#include <unistd.h>


extern "C" void app_main(void)
{
    NetworkManager::Init();

    while(CONNECTED != 1)
    {
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }

    MbClient clientCOM3VA("192.168.0.101");
    clientCOM3VA.SetIdentifier("3VA");
    clientCOM3VA.SetRequests({
        MbClient::RequestType::UP_TIME,
        MbClient::RequestType::ORDER_ID,
        MbClient::RequestType::SER_NUM,
        MbClient::RequestType::INT_L1,
        MbClient::RequestType::INT_L2,
        MbClient::RequestType::INT_L3,
        MbClient::RequestType::INT_AVG,
        MbClient::RequestType::TMP_ETU,
    });

    MbClient clientCOM("192.168.0.102");
    clientCOM.SetDeviceType(MbClient::DeviceType::DT_COM);
    clientCOM.SetIdentifier("COM800");
    clientCOM.SetRequests({
        MbClient::RequestType::UP_TIME,
        MbClient::RequestType::ORDER_ID,
        MbClient::RequestType::SER_NUM,
        MbClient::RequestType::TMP,
        //MbClient::RequestType::INT_L1,
        //MbClient::RequestType::INT_L2,
        //MbClient::RequestType::INT_L3,
        //MbClient::RequestType::INT_AVG,
    });
    
    MbClient clientPOC("192.168.0.192");
    clientPOC.SetDeviceType(MbClient::DeviceType::DT_POC);
    clientPOC.SetIdentifier("POC1000");
    clientPOC.SetRequests({
        MbClient::RequestType::TMP,
        MbClient::RequestType::ORDER_ID,
        MbClient::RequestType::SER_NUM,
        //MbClient::RequestType::INT_L1,
        //MbClient::RequestType::VTG_L1,
    });

    MbManager manager;
    manager.AddClient(clientCOM3VA);
    manager.AddClient(clientCOM);
    manager.AddClient(clientPOC);
    manager.Run();

    while(true) 
    {
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
}
