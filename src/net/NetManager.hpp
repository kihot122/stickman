#pragma once

#include <thread>
#include <mutex>
#include <map>
#include <atomic>

#include "../aux/concurrentqueue.hpp"
#include "GamePacket.hpp"
#include "GameCommand.hpp"

class NetManager
{
    moodycamel::ConcurrentQueue<GamePacket *> SendQueue;
    moodycamel::ConcurrentQueue<GamePacket *> ReceiveQueue;
    moodycamel::ConcurrentQueue<GameCommand *> CommandQueue;

    moodycamel::ConcurrentQueue<int> SocketsNew;
    moodycamel::ConcurrentQueue<int> SocketsDel;

    std::thread *ThreadListen;
    std::thread *ThreadSend;
    std::thread *ThreadRun;

    std::string Port;
    std::map<int, std::thread *> ReceiveThreadPool;
    std::map<int, std::atomic<bool>> ReceiveThreadPoolFlag;
    std::mutex ReceiveThreadPoolLock;

    void Listen();
    void Send();
    void Receive(int SocketFd);

public:
    NetManager(std::string Port);
    ~NetManager();

    void Run();
    void Push(GamePacket *Packet);
    std::vector<GamePacket *> Pull();

    int GetSocketNew();
    int GetSocketDel();

    void Connect(std::string Address, std::string Port);
    void Disconnect(int SocketFd);
};