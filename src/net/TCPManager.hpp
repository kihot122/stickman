#pragma once

#include <string>

#include "GamePacket.hpp"

class TCPServer
{
    int SocketFd;
public:
    TCPServer(std::string Port);
    ~TCPServer();

    void Run();
};

class TCPClient
{
};