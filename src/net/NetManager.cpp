#include <stdexcept>
#include <chrono>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <netdb.h>

#include "NetManager.hpp"

void NetManager::Listen()
{
    int SocketFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in ServerAddress{};
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(std::stoi(Port));
    ServerAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(SocketFd, reinterpret_cast<sockaddr *>(&ServerAddress), sizeof(ServerAddress)))
        throw std::runtime_error("Unable to bind port.");

    if (listen(SocketFd, 16))
        throw std::runtime_error("Unable to listen on port.");

    ListenSocketFd = SocketFd;

    ReadyUp.unlock();

    while (true)
    {
        sockaddr_in ClientAddress{};
        socklen_t ClientAddressSize;

        int Result = accept(SocketFd, reinterpret_cast<sockaddr *>(&ClientAddress), &ClientAddressSize);
        if (Result == -1)
        {
            std::cout << "Error" << errno;
            break;
        }

        SocketsNew.enqueue(Result);

        ReceiveThreadPoolLock.lock();
        ReceiveThreadPoolFlag.emplace(Result, false);
        ReceiveThreadPool.emplace(Result, new std::thread([this, Result] { this->Receive(Result); }));
        ReceiveThreadPoolLock.unlock();
    }
}

void NetManager::Send()
{
    using namespace std::chrono_literals;
    GamePacket *Packet;

    while (true)
    {
        if (ThreadSendClose)
            break;

        std::this_thread::sleep_for(1ms);

        if (SendQueue.try_dequeue(Packet))
        {
            uint64_t Size = sizeof(GamePacketType) + sizeof(uint64_t) + Packet->Data.size();
            uint8_t *Data = new uint8_t[Size];

            std::memcpy(Data, &Packet->Type, sizeof(GamePacketType));
            std::memcpy(Data + sizeof(GamePacketType), &static_cast<const uint64_t &>(Packet->Data.size()), sizeof(uint64_t));
            std::memcpy(Data + sizeof(GamePacketType) + sizeof(uint64_t), Packet->Data.data(), Packet->Data.size());

            write(Packet->Socket, Data, Size);

            delete[] Data;
            delete Packet;
        }
    }
}

void NetManager::Receive(int SocketFd)
{
    uint64_t Size;
    GamePacket *Packet;

    while (true)
    {
        Packet = new GamePacket;
        Packet->Socket = SocketFd;

        if (read(SocketFd, &Packet->Type, sizeof(GamePacketType)) <= 0)
            break;
        if (read(SocketFd, &Size, sizeof(uint64_t)) <= 0)
            break;
        Packet->Data.resize(Size);

        if (read(SocketFd, Packet->Data.data(), sizeof(Size)) <= 0)
            break;

        ReceiveQueue.enqueue(Packet);
    }

    delete Packet;
    ReceiveThreadPoolFlag[SocketFd] = true;
}

NetManager::NetManager(std::string Port) : Port(Port)
{
    ThreadSendClose = false;
    ReadyUp.lock();

    ThreadListen = new std::thread([this] { this->Listen(); });
    ThreadSend = new std::thread([this] { this->Send(); });
    ThreadRun = new std::thread([this] { this->Run(); });

    ReadyUp.lock();
}

NetManager::~NetManager()
{
    ReadyUp.unlock();

    CommandQueue.enqueue(new GameCommandNetShutdown());
    ThreadRun->join();
    delete ThreadRun;
}

void NetManager::Run()
{
    using namespace std::chrono_literals;

    while (true)
    {
        std::this_thread::sleep_for(1ms);

        GameCommand *Command;
        if (CommandQueue.try_dequeue(Command))
        {
            switch (Command->Type)
            {
                case GameCommandType::NET_CONN:
                {
                    GameCommandNetConnect *RealCommand = static_cast<GameCommandNetConnect *>(Command);

                    addrinfo *resolved, hints{};
                    hints.ai_family = AF_INET;
                    hints.ai_socktype = SOCK_STREAM;

                    int res = getaddrinfo(RealCommand->Address.c_str(), RealCommand->Port.c_str(), &hints, &resolved);
                    if (res or !resolved)
                        throw std::runtime_error("Could not resolve address.");

                    int Result = socket(resolved->ai_family, resolved->ai_socktype, 0);
                    if (Result == -1)
                        throw std::runtime_error("Socket creation failed.");

                    if (connect(Result, resolved->ai_addr, resolved->ai_addrlen) == -1)
                        throw std::runtime_error("Connection failed.");

                    freeaddrinfo(resolved);

                    SocketsNew.enqueue(Result);

                    ReceiveThreadPoolLock.lock();
                    ReceiveThreadPoolFlag.emplace(Result, false);
                    ReceiveThreadPool.emplace(Result, new std::thread([this, Result] { this->Receive(Result); }));
                    ReceiveThreadPoolLock.unlock();

                    break;
                }
                case GameCommandType::NET_DISCONN:
                {
                    GameCommandNetDisconnect *RealCommand = static_cast<GameCommandNetDisconnect *>(Command);

                    shutdown(RealCommand->SocketFd, SHUT_RDWR);
                    close(RealCommand->SocketFd);

                    break;
                }
                case GameCommandType::NET_SHUTDOWN:
                {
                    GameCommandNetShutdown *RealCommand = static_cast<GameCommandNetShutdown *>(Command);

                    ThreadSendClose = true;
                    ThreadSend->join();
                    delete ThreadSend;

                    shutdown(ListenSocketFd, SHUT_RDWR);
                    close(ListenSocketFd);
                    ThreadListen->join();
                    delete ThreadListen;

                    for (auto &Pair : ReceiveThreadPool)
                    {
                        shutdown(Pair.first, SHUT_RDWR);
                        close(Pair.first);

                        Pair.second->join();
                        delete Pair.second;
                    }
                    ReceiveThreadPool.clear();
                    ReceiveThreadPoolFlag.clear();

                    GamePacket *Temp;
                    while (SendQueue.try_dequeue(Temp))
                        delete Temp;

                    while (ReceiveQueue.try_dequeue(Temp))
                        delete Temp;

                    GameCommand *Cmd;
                    while (CommandQueue.try_dequeue(Cmd))
                        delete Cmd;

                    delete RealCommand;
                    return;
                }
            }

            delete Command;
        }

        for (auto iter = ReceiveThreadPoolFlag.begin(); iter != ReceiveThreadPoolFlag.end();)
        {
            if (iter->second)
            {
                SocketsDel.enqueue(iter->first);

                ReceiveThreadPool[iter->first]->join();
                delete ReceiveThreadPool[iter->first];

                ReceiveThreadPoolLock.lock();
                ReceiveThreadPool.erase(iter->first);
                iter = ReceiveThreadPoolFlag.erase(iter);
                ReceiveThreadPoolLock.unlock();

                continue;
            }
            iter++;
        }
    }
}

void NetManager::Push(GamePacket *Packet)
{
    SendQueue.enqueue(Packet);
}

std::vector<GamePacket *> NetManager::Pull()
{
    std::vector<GamePacket *> BulkData(ReceiveQueue.size_approx());
    size_t ActualSize = ReceiveQueue.try_dequeue_bulk(BulkData.data(), BulkData.size());
    BulkData.resize(ActualSize);
    return BulkData;
}

int NetManager::GetSocketNew()
{
    int Socket;
    if (SocketsNew.try_dequeue(Socket))
        return Socket;

    return -1;
}

int NetManager::GetSocketDel()
{
    int Socket;
    if (SocketsDel.try_dequeue(Socket))
        return Socket;

    return -1;
}

void NetManager::Connect(std::string Address, std::string Port)
{
    CommandQueue.enqueue(new GameCommandNetConnect(Address, Port));
}

void NetManager::Disconnect(int SocketFd)
{
    CommandQueue.enqueue(new GameCommandNetDisconnect(SocketFd));
}