#include "MbClient.hpp"

// STL
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "lwip/netdb.h"

#include "esp_log.h"

#define TAG "MbClient"

namespace 
{
    enum MbErrorCode
    {
        ILLEGAL_FUNCTION = 0x01,
        ILLEGAL_DATA_ADDRESS,
        ILLEGAL_DATA_VALUE,
        SLAVE_DEVICE_FAILURE
    };

    const std::map<MbClient::RequestType, std::array<uint8_t, 5>> requestTableCOM
    {
        //
        // Drop the hardcoded values
        { MbClient::RequestType::UP_TIME , { 0xFF, 0x00, 0x97, 0x00, 0x04 } }, // v
        { MbClient::RequestType::TMP     , { 0xFF, 0x40, 0x05, 0x00, 0x02 } },
        { MbClient::RequestType::TMP_ETU , { 0x01, 0x3F, 0xFF, 0x00, 0x02 } },
        { MbClient::RequestType::VTG_L1  , { 0xFF, 0x4F, 0xFD, 0x00, 0x02 } },
        { MbClient::RequestType::VTG_L2  , { 0xFF, 0x4F, 0xFD, 0x00, 0x02 } },
        { MbClient::RequestType::VTG_L3  , { 0xFF, 0x4F, 0xFD, 0x00, 0x02 } },
        { MbClient::RequestType::VTG_AVG , { 0xFF, 0x4F, 0xFD, 0x00, 0x02 } },
        { MbClient::RequestType::INT_L1  , { 0x01, 0x00, 0x01, 0x00, 0x02 } }, // v
        { MbClient::RequestType::INT_L2  , { 0x01, 0x00, 0x03, 0x00, 0x02 } }, // v
        { MbClient::RequestType::INT_L3  , { 0x01, 0x00, 0x05, 0x00, 0x02 } }, // v
        { MbClient::RequestType::INT_AVG , { 0x01, 0x00, 0x17, 0x00, 0x02 } }, // v
        { MbClient::RequestType::ORDER_ID, { 0xFF, 0xFA, 0x01, 0x00, 0x1B } }, // v
        { MbClient::RequestType::SER_NUM , { 0xFF, 0xFA, 0x01, 0x00, 0x1B } }  // v

    };

    const std::map<MbClient::RequestType, std::array<uint8_t, 5>> requestTablePOC
    {
        { MbClient::RequestType::TMP     , { 0x01, 0x0B, 0xFF, 0x00, 0x02 } }, // v
        { MbClient::RequestType::VTG_L1  , { 0x01, 0x0C, 0x09, 0x00, 0x02 } }, // v
        { MbClient::RequestType::VTG_L2  , { 0xFF, 0x4F, 0xFD, 0x00, 0x02 } },
        { MbClient::RequestType::VTG_L3  , { 0xFF, 0x4F, 0xFD, 0x00, 0x02 } },
        { MbClient::RequestType::VTG_AVG , { 0xFF, 0x4F, 0xFD, 0x00, 0x02 } },
        { MbClient::RequestType::INT_L1  , { 0x01, 0x0C, 0x03, 0x00, 0x02 } }, // v
        { MbClient::RequestType::INT_L2  , { 0xFF, 0x4F, 0xFD, 0x00, 0x02 } },
        { MbClient::RequestType::INT_L3  , { 0xFF, 0x4F, 0xFD, 0x00, 0x02 } },
        { MbClient::RequestType::INT_AVG , { 0x01, 0x0C, 0x05, 0x00, 0x02 } }, // v
        { MbClient::RequestType::ORDER_ID, { 0xFF, 0x00, 0x02, 0x00, 0x0A } }, // v
        { MbClient::RequestType::SER_NUM , { 0xFF, 0x00, 0x0C, 0x00, 0x08 } }  // v
    };

    std::array<uint8_t, 12> ProcessRequest(MbClient::RequestType msgId, MbClient::DeviceType devType, uint16_t transactionId)
    {
        const auto& requestTable = (MbClient::DeviceType::DT_COM == devType) ? requestTableCOM : requestTablePOC;

        const uint8_t transactionIdLeft   = transactionId >> 8;
        const uint8_t transactionIdRight  = transactionId & 0xFF;

        std::array<uint8_t, 12> processedReq = { transactionIdLeft, transactionIdRight, 0x00, 0x00, 0x00, 0x06, requestTable.at(msgId)[0], 0x03 };
        processedReq[8]  = requestTable.at(msgId)[1];
        processedReq[9]  = requestTable.at(msgId)[2];
        processedReq[10] = requestTable.at(msgId)[3];
        processedReq[11] = requestTable.at(msgId)[4];

        return processedReq;
    }

    const char* MbErrorString(uint32_t errorCode)
    {
        switch(errorCode)
        {
        case ILLEGAL_FUNCTION:
            return "Illegal function";
        case ILLEGAL_DATA_ADDRESS:
            return "Illegal data address";
        case ILLEGAL_DATA_VALUE:
            return "Illegal data value";
        case SLAVE_DEVICE_FAILURE:
            return "Slave device failure";
        default:
            return "InvalidErrorCode";
        }
    }
}

MbClient::MbClient(std::string serverAddr, int port)
{
    // Create socket
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket < 0)
    {
        printf("Failed to create socket\n");
        vTaskDelete(NULL);
    }

    // Configure server address
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverAddr.c_str(), &m_serverAddr.sin_addr);

    // Connect to Modbus TCP server
    if (connect(m_socket, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr)) < 0)
    {
        printf("Failed to connect to server\n");
        close(m_socket);
        vTaskDelete(NULL);
    }
}

void MbClient::SetRequests(std::vector<RequestType> requests)
{
    m_requests = requests;
}

std::vector<MbClient::RequestType> MbClient::GetRequests()
{
    return m_requests;
}

void MbClient::SetDeviceType(DeviceType devType)
{
    m_deviceType = devType;
}

MbClient::DeviceType MbClient::GetDeviceType()
{
    return m_deviceType;
}

void MbClient::SetIdentifier(std::string id)
{
    m_identifier = id;
}

std::string MbClient::GetIdentifier()
{
    return m_identifier;
}

void MbClient::SendReceiveRequest(RequestType requestId)
{
    m_transactionCounter++;

    std::array<uint8_t, 12> requestArray = ProcessRequest(requestId, m_deviceType, m_transactionCounter);

    if (send(m_socket, requestArray.data(), 12, 0) < 0)
    {
        ESP_LOGW(TAG, "Modbus request frame failed to send");
        return;
    }
    
    ESP_LOGI(TAG, "Modbus request frame sent successfully");

    int byteCount = recv(m_socket, m_receiveBuffer, sizeof(m_receiveBuffer), 0);
    ESP_LOGI(TAG, "Response byte count: %d", byteCount);
    
    printf("Modbus response RAW frame: ");
    for (int i = 0; i < byteCount; i++)
        printf("0x%02X ", m_receiveBuffer[i]);
    printf("\n");
}

ByteVector MbClient::ProcessResponse()
{
    uint16_t transactionCounter = (m_receiveBuffer[0] << 8) + m_receiveBuffer[1];
    
    if(transactionCounter != m_transactionCounter)
    {
        ESP_LOGI(TAG, "INVALID TRANSACTION ID COUNTER %x != ACT:%x", transactionCounter, m_transactionCounter);
        return {};
    }

    uint8_t functionCode = m_receiveBuffer[7];
    if(functionCode > 80)
    {
        ESP_LOGW(TAG, "MODBUS ERROR: 0x%02x - %s", functionCode, MbErrorString(m_receiveBuffer[8]));
        return {};
    }

    const size_t lenght = m_receiveBuffer[8];
    
    ByteVector responseValues;
    responseValues.resize(lenght);
    
    for(int i = 9; i < 9 + lenght; i++)
        responseValues[i-9] = m_receiveBuffer[i];

    // TODO: Move this nonsense to a log class -> LOG::INFO(responseValues)
    printf("\u001b[31mResponse:");
    for (uint8_t byte : responseValues)
        printf("\u001b[33m0x%02X\u001b[37m ", byte);
    printf("\n");

    return responseValues;
}
