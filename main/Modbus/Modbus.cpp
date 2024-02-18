#include "Modbus.h"
#include "MbClient.h"

namespace
{
    MbClient* client = nullptr; 
}

void MbInit()
{
    client = new MbClient();
}

void MbSendRequest()
{
    client->SendReceiveRequest();
}

void MbDestroy()
{
    delete client;
}