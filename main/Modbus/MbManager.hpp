#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <thread>

class MbClient;

using ClientsVector = std::vector<MbClient>;

class MbManager
{
public:
    MbManager() = default;
    ~MbManager();
    void Run();
    void AddClient(MbClient client);

private:
    void RemoveClient(int id);

    static void Listen(ClientsVector* clients);
private:
    ClientsVector m_clients;

    std::unique_ptr<std::thread> m_uqThread;

    bool m_running = false;
};
