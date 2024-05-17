#include <cstdint>
#include <string>

#include <vector>
#include <array>

#include "lwip/sockets.h"

#define MB_TCP_DEFAULT_PORT 502

using ByteVector = std::vector<uint8_t>;

constexpr std::array<const char*, 14> REQUEST_TYPE_STR =
{
    "NONE",
    "UP_TIME",
    "TMP",
    "TMP",
    "VTG_L1",
    "VTG_L2",
    "VTG_L3",
    "VTG_AVG",
    "INT_L1",
    "INT_L2",
    "INT_L3",
    "INT_AVG",
    "ORDER_ID",
    "SER_NUM"
};

class MbClient
{
public:
    MbClient();
    MbClient(std::string serverAddr, int port = MB_TCP_DEFAULT_PORT);
    ~MbClient() = default;

    enum RequestType {
        NONE = 0,
        UP_TIME,
        TMP,
        TMP_ETU,
        VTG_L1,
        VTG_L2,
        VTG_L3,
        VTG_AVG,
        INT_L1,
        INT_L2,
        INT_L3,
        INT_AVG,
        ORDER_ID,
        SER_NUM
    };

    enum DeviceType {
        DT_COM,
        DT_POC
    };

    void SetRequests(std::vector<RequestType> requests);
    std::vector<RequestType> GetRequests();

    void SendReceiveRequest(RequestType requestId);
    ByteVector ProcessResponse();

    void SetDeviceType(DeviceType devType);
    DeviceType GetDeviceType();

    void SetIdentifier(std::string id);
    std::string GetIdentifier();

    inline uint8_t GetId() { return m_slaveAddr; }

private:
    uint8_t m_slaveAddr;
    uint16_t m_transactionCounter = 0x0000;

    int m_socket;
    char m_receiveBuffer[128];

    struct sockaddr_in m_serverAddr;

    DeviceType m_deviceType = DT_COM;

    std::string m_identifier;

    std::vector<RequestType> m_requests;
};