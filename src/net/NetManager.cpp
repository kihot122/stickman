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

    while (true)
    {
        sockaddr_in ClientAddress{};
        socklen_t ClientAddressSize;

        int Result = accept(SocketFd, reinterpret_cast<sockaddr *>(&ClientAddress), &ClientAddressSize);
        if (Result == -1)
        {
            std::cerr << "Error accepting new connection.\n";
            continue;
        }

        SocketsNew.enqueue(Result);
        ReceiveThreadPool.push_back(new std::thread([this, Result] { this->Receive(Result); }));
    }
}

void NetManager::Send()
{
    using namespace std::chrono_literals;
    GamePacket *Packet;

    while (true)
    {
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

    while (true)
    {
        GamePacket *Packet = new GamePacket;
        Packet->Socket = SocketFd;

        read(SocketFd, &Packet->Type, sizeof(GamePacketType));
        read(SocketFd, &Size, sizeof(uint64_t));
        Packet->Data.resize(Size);

        read(SocketFd, Packet->Data.data(), sizeof(Size));

        ReceiveQueue.enqueue(Packet);
    }
}

NetManager::NetManager(std::string Port) : Port(Port) {}
NetManager::~NetManager() {}

void NetManager::Run()
{
    std::thread ThreadListen = std::thread([this] { this->Listen(); });
    std::thread ThreadSend = std::thread([this] { this->Send(); });

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
                    ReceiveThreadPool.push_back(new std::thread([this, Result] { this->Receive(Result); }));

                    break;
                }
                case GameCommandType::NET_DISCONN:
                {
                    break;
                }
            }

            delete Command;
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

void NetManager::Disconnect(std::string Address)
{
}