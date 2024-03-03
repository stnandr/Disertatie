#include "NetworkManager/NetworkManager.h"
#include "NetworkManager/Tests.h"
#include "Modbus/Modbus.h"
#include "FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

void app_main(void)
{
    NetworkManagerInit();

    while(CONNECTED != 1)
    {
    usleep(1000);
    }


    //NetworkManagerRun();
    MbInit();
    MbSendRequest();
}


