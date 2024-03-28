#include "MbManager.h"
#include "MbClient.h"

#include <thread>
#include <chrono>

//
//namespace
//{
//    MbClient* client = nullptr; 
//}
//
//void MbInit()
//{
//    ::client = new MbClient("192.168.1.101" , 0xFF);
//}
//
//void MbSendRequest()
//{
//    ::client->SendReceiveRequest("OperatingHours");
//}
//
//void MbDestroy()
//{
//    delete ::client;
//}

MbManager::~MbManager() {
    m_uqThread->join();
    //
    // When we get here kill the thread
}

void MbManager::Run()
{
    if (m_running)
        return;

    m_running = true;

    m_uqThread = std::make_unique<std::thread>(Listen, &m_clients);
    m_uqThread->detach();
}

void MbManager::AddClient(MbClient client)
{
    m_clients.push_back(client);
}

void MbManager::RemoveClient(int id)
{
    //m_clients.erase(id);
}

void MbManager::Listen(ClientsVector* clients)
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for (MbClient& client : *clients) {
            client.SendReceiveRequest("OperatingHours");
            ByteVector result = client.ProcessResponse();
        }
    }
}
