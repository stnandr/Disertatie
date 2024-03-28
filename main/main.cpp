#include "NetworkManager/NetworkManager.h"
#include "Modbus/MbManager.h"
#include "Modbus/MbClient.h"

#include <unistd.h>

extern "C" void app_main(void)
{
    NetworkManager::Init();

    while(CONNECTED != 1)
    {
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }

    MbClient client1("192.168.1.101" , 0xFF);
    MbManager manager;
    manager.AddClient(client1);
    manager.Run();

    while(true) 
    {
        std::this_thread::sleep_for (std::chrono::seconds(1));
        printf("Im in -------------------------------------------------------\n");
    }
    //MbClient client2("192.168.1.101" , 0xFF);
    //MbClient client3("192.168.1.101" , 0xFF);
}
