#include <cstdint>
#include <string>

#include "../aux/concurrentqueue.hpp"
#include "TCPManager.hpp"
#include "UDPManager.hpp"
#include "GamePacket.hpp"

class NetServer
{
public:
    NetServer(std::string TcpPort, std::string UdpPort);
    ~NetServer();

    void Run();
};

class NetClient
{
    moodycamel::ConcurrentQueue<GamePacket> SendQueue;
    moodycamel::ConcurrentQueue<GamePacket> ReceiveQueue;

public:
    NetClient(std::string Address, std::string Port);
    ~NetClient();

    void Run();
};