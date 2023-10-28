#include "NetworkManager/NetworkManager.h"
#include "Test.h"

void app_main(void)
{
    NetworkManagerInit();
    printf("%d", TestFunction());
}


