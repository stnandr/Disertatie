#include "MbClient.h"

// STL
#include <array>
#include <string>
#include <map>
#include <thread>

#include "lwip/netdb.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define MB_TCP_DEFAULT_PORT 502
#define TAG "ModbusClient"


namespace {
    std::map<std::string, std::array<uint8_t, 4>> requestTable{
        //
        // Drop the hardcoded values
        { "OperatingHours", { 0x4B, 0xFD, 0x00, 0x02 } }
    };

    std::array<uint8_t, 12> ProcessRequest(std::string msgId, uint8_t slaveId, uint16_t transactionId) {
        uint8_t transactionIdLeft = transactionId >> 8;
        uint8_t transactionIdRight  = transactionId & 0xFF;

        std::array<uint8_t, 12> processedReq = { transactionIdLeft, transactionIdRight, 0x00, 0x00, 0x00, 0x06, slaveId, 0x03 };
        processedReq[8]  = requestTable["OperatingHours"][0];
        processedReq[9]  = requestTable["OperatingHours"][1];
        processedReq[10] = requestTable["OperatingHours"][2];
        processedReq[11] = requestTable["OperatingHours"][3];

        return processedReq;
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

MbClient::~MbClient()
{
}

void MbClient::SendReceiveRequest(std::string requestId)
{
    while(true) {
        usleep(3000000);

        std::array<uint8_t, 12> requestArray = ProcessRequest(requestId, m_slaveAddr, m_transactionCounter);

        if (send(m_socket, requestArray.data(), 12, 0) < 0)
        {
            
        }

        m_transactionCounter++;


        //struct sockaddr_in sourceAddr;
        //uint32_t addrLen = sizeof(sourceAddr);
        //int clientSocket = accept(m_socket, (struct sockaddr *)&sourceAddr, &addrLen);

        //if (clientSocket < 0)
        //{
        //	ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
        //	continue;
        //}

        //int byteCount = recv(clientSocket, m_receiveBuffer, sizeof(m_receiveBuffer) - 1, 0);
        //ESP_LOGI(TAG, "ByteCount: %d", byteCount);

        //for (int i = 0; i < 128 && m_receiveBuffer[i] != '\0'; i++)
        //    ESP_LOGI(TAG, "%X", m_receiveBuffer[i]);
    }
}

void MbClient::ProcessResponse()
{
    //std::thread thread (
    //    [&] { ListeningTask(); }
    //);

    //thread.join();
}

void MbClient::ListeningTask()
{
    usleep(1000000);

    int byteCount = recv(m_socket, m_receiveBuffer, sizeof(m_receiveBuffer) - 1, 0);
    ESP_LOGI(TAG, "%s", m_receiveBuffer);
}

