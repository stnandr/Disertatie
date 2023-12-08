#include "NetworkManager/NetworkManager.h"
#include "NetworkManager/Tests.h"
#include "Test.h"

#include <stdio.h>

void app_main(void)
{
    NetworkManagerInit();
    NetworkManagerRun();
    printf("%d", TestFunction());
    //TEST_Echo();
}


