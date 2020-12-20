#pragma once

#include <thread>
#include <mutex>
#include <map>
#include "../aux/concurrentqueue.hpp"
#include "GamePacket.hpp"
#include "GameCommand.hpp"

class NetManager
{
    moodycamel::ConcurrentQueue<GamePacket *> SendQueue;
    moodycamel::ConcurrentQueue<GamePacket *> ReceiveQueue;
    moodycamel::ConcurrentQueue<GameCommand *> CommandQueue;

    std::string Port;
    std::vector<std::thread *> ReceiveThreadPool;

    void Listen();
    void Send();
    void Receive(int SocketFd);

public:
    NetManager(std::string Port);
    ~NetManager();

    void Run();
    void Push(GamePacket *Packet);
    std::vector<GamePacket *> Pull();

    void Connect(std::string Address, std::string Port);
    void Disconnect(std::string Address);
};