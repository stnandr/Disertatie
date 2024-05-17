#include "MbManager.hpp"
#include "MbClient.hpp"

#include <thread>
#include <chrono>

#include "../Mqtt/MqttClient.hpp"

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

namespace
{
    void PostMessageOverMQTT(ByteVector byteVec, std::string identifier, MbClient::DeviceType devType, MbClient::RequestType reqType)
    {
        if (byteVec.size() <= 1)
            return;

        MqttClient* mqttInstance = MqttClient::GetInstance();

        std::string message;
        std::string topic = identifier.append("/").append(REQUEST_TYPE_STR[reqType]);

        switch(reqType)
        {
        case MbClient::RequestType::UP_TIME:
        {
            uint64_t upTime = ((uint64_t)byteVec[4] << 56)
                            | ((uint64_t)byteVec[5] << 48)
                            | ((uint64_t)byteVec[6] << 40)
                            | ((uint64_t)byteVec[7] << 32)
                            | ((uint64_t)byteVec[0] << 24)
                            | ((uint64_t)byteVec[1] << 16)
                            | ((uint64_t)byteVec[2] << 8)
                            |  (uint64_t)byteVec[3];

            message.append(std::to_string(upTime));
            printf("\n\n%llu\n\n", upTime);

            break;
        }
        case MbClient::INT_L1:
        case MbClient::INT_L2:
        case MbClient::INT_L3:
        case MbClient::INT_AVG:
        case MbClient::VTG_L1:
        case MbClient::VTG_L2:
        case MbClient::VTG_L3:
        case MbClient::VTG_AVG:
        case MbClient::TMP_ETU:
        case MbClient::RequestType::TMP:
        {
            uint32_t floatValInt;
            floatValInt = ((uint32_t)byteVec[0] << 24)
                        | ((uint32_t)byteVec[1] << 16)
                        | ((uint32_t)byteVec[2] <<  8)
                        |  (uint32_t)byteVec[3];

            float* floatVal = (float*)((void*)(&floatValInt));
            message.append(std::to_string(*floatVal));

            printf("\n\n%f\n\n", *floatVal);

            break;
        }
        case MbClient::SER_NUM:
        case MbClient::ORDER_ID:
        {
            if (MbClient::DeviceType::DT_POC == devType)
            {
                size_t size = 0;
                if (MbClient::ORDER_ID == reqType)
                    size = 20;
                else if (MbClient::SER_NUM == reqType)
                    size = 16;

                std::string strCharArray;
                strCharArray.resize(size);

                for (int i = 0; i < size; i++)
                    strCharArray[i] = byteVec[i];

                message.append(strCharArray);
            }
            else if (MbClient::DeviceType::DT_COM == devType)
            {
                size_t size = 0;
                size_t offset = 0;
                if (MbClient::ORDER_ID == reqType)
                {
                    size = 20;
                    offset = 2;
                }
                else if (MbClient::SER_NUM == reqType)
                {
                    size = 16;
                    offset = 22;
                }

                std::string strCharArray;
                strCharArray.resize(size);

                for (int i = offset; i < offset + size; i++)
                    strCharArray[i - offset] = byteVec[i];

                message.append(strCharArray);
            }

            break;
        }
        default:
        {

            break;
        }
        }

        mqttInstance->Publish(message, topic);
    }
}

void MbManager::Listen(ClientsVector* clients)
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for (MbClient& client : *clients)
        {
            for (const MbClient::RequestType& req : client.GetRequests())
            {
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                client.SendReceiveRequest(req);
                ByteVector result = client.ProcessResponse();

                PostMessageOverMQTT(result, client.GetIdentifier(), client.GetDeviceType(), req);
            }
        }
    }
}
