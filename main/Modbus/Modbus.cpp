#include "Modbus.h"
#include "MbClient.h"

namespace
{
    MbClient* client = nullptr; 
}

void MbInit()
{
    ::client = new MbClient("192.168.1.101" , 0xFF);
}

void MbSendRequest()
{
    ::client->ProcessResponse();
    ::client->SendReceiveRequest("OperatingHours");
   
}

void MbDestroy()
{
    delete ::client;
}