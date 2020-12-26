#pragma once

#include <cstdint>
#include <string>

enum class GameCommandType : uint8_t
{
    NET_CONN,
    NET_DISCONN,
    NET_SHUTDOWN,
};

struct GameCommand
{
    GameCommandType Type;
    virtual ~GameCommand() {}
};

struct GameCommandNetConnect : public GameCommand
{
    std::string Address;
    std::string Port;

    GameCommandNetConnect(std::string Address, std::string Port) : Address(Address), Port(Port)
    {
        Type = GameCommandType::NET_CONN;
    }
};

struct GameCommandNetDisconnect : public GameCommand
{
    int SocketFd;

    GameCommandNetDisconnect(int SocketFd) : SocketFd(SocketFd)
    {
        Type = GameCommandType::NET_DISCONN;
    }
};

struct GameCommandNetShutdown : public GameCommand
{
    GameCommandNetShutdown()
    {
        Type = GameCommandType::NET_SHUTDOWN;
    }
};