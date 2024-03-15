#include "NetworkManager/NetworkManager.h"
#include "Modbus/Modbus.h"

#include <unistd.h>

void app_main(void)
{
    NetworkManagerInit();

    while(CONNECTED != 1)
    {
        usleep(1000);
    }

    MbInit();
    MbSendRequest();
}
