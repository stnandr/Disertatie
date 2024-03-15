#include "MbClient.h"

// STL
#include <array>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "lwip/netdb.h"

#include "esp_log.h"

#define MB_TCP_DEFAULT_PORT 502
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

    std::map<std::string, std::array<uint8_t, 4>> requestTable
    {
        //
        // Drop the hardcoded values
        { "OperatingHours", { 0x4B, 0xFD, 0x00, 0x02 } },
        { "Testing", { 0x4F, 0xFD, 0x00, 0x02 } }
    };

    std::array<uint8_t, 12> ProcessRequest(std::string msgId, uint8_t slaveId, uint16_t transactionId)
    {
        uint8_t transactionIdLeft = transactionId >> 8;
        uint8_t transactionIdRight  = transactionId & 0xFF;

        std::array<uint8_t, 12> processedReq = { transactionIdLeft, transactionIdRight, 0x00, 0x00, 0x00, 0x06, slaveId, 0x03 };
        processedReq[8]  = requestTable[msgId][0];
        processedReq[9]  = requestTable[msgId][1];
        processedReq[10] = requestTable[msgId][2];
        processedReq[11] = requestTable[msgId][3];

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

MbClient::MbClient(std::string serverAddr, uint8_t slaveAddr)
    : m_slaveAddr(slaveAddr)
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
    m_serverAddr.sin_port = htons(MB_TCP_DEFAULT_PORT);
    inet_pton(AF_INET, serverAddr.c_str(), &m_serverAddr.sin_addr);

    // Connect to Modbus TCP server
    if (connect(m_socket, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr)) < 0)
    {
        printf("Failed to connect to server\n");
        close(m_socket);
        vTaskDelete(NULL);
    }
}

void MbClient::SendReceiveRequest(std::string requestId)
{
    while(true)
    {
        usleep(3000000);

        m_transactionCounter++;

        std::array<uint8_t, 12> requestArray = ProcessRequest(requestId, m_slaveAddr, m_transactionCounter);

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

        ProcessResponse();
    }
}

void MbClient::ProcessResponse()
{
    uint16_t transactionCounter = (m_receiveBuffer[0] << 8) + m_receiveBuffer[1];
    
    if(transactionCounter != m_transactionCounter)
    {
        ESP_LOGI(TAG, "INVALID TRANSACTION ID COUNTER %x != ACT:%x", transactionCounter, m_transactionCounter);
        return;
    }

    uint8_t functionCode = m_receiveBuffer[7];
    if(functionCode > 80)
    {
        ESP_LOGW(TAG, "MODBUS ERROR: 0x%02x - %s", functionCode, MbErrorString(m_receiveBuffer[8]));
        return;
    }

    const size_t lenght = m_receiveBuffer[8];
    std::vector<uint8_t> responseValues;
    responseValues.resize(lenght);
    for(int i = 9; i < 9 + lenght; i++)
    responseValues[i-9] = m_receiveBuffer[i];

    std::string response;

    printf("\u001b[31mResponse:");
    for (uint8_t byte : responseValues)
        printf("\u001b[33m0x%02X\u001b[37m ", byte);
    printf("\n");

}
