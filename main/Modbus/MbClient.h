#include <cstdint>
#include <string>

#include "lwip/sockets.h"

class MbClient
{
public:
    MbClient() = delete;
    MbClient(std::string serverAddr, uint8_t slaveAddr);
    ~MbClient();

    void SendReceiveRequest(std::string requestId);
    void ProcessResponse();

private:
    void ListeningTask();

private:
    uint8_t m_slaveAddr;
    uint16_t m_transactionCounter = 0x0001;

    int m_socket;

    char m_receiveBuffer[128];

    struct sockaddr_in m_serverAddr;
};