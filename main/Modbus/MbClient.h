#include <cstdint>
#include <string>

#include <vector>

#include "lwip/sockets.h"

#define MB_TCP_DEFAULT_PORT 502

using ByteVector = std::vector<uint8_t>;

class MbClient
{
public:
    MbClient();
    MbClient(std::string serverAddr, uint8_t slaveAddr, int port = MB_TCP_DEFAULT_PORT);
    ~MbClient() = default;

    void SendReceiveRequest(std::string requestId);
    ByteVector ProcessResponse();

    inline uint8_t GetId() { return m_slaveAddr; }
private:
    uint8_t m_slaveAddr;
    uint16_t m_transactionCounter = 0x0000;

    int m_socket;

    char m_receiveBuffer[128];

    struct sockaddr_in m_serverAddr;
};