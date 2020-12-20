#pragma once

#include <cstdint>
#include <string>
#include <vector>

enum class GamePacketType : uint8_t
{
    CLIENT_INFO,
    SERVER_INFO,
    CLIENT_OK,
    CLIENT_MESSAGE,
    SERVER_MESSAGE,
};

struct GamePacket
{
    GamePacketType Type;
    int Socket;
    std::vector<uint8_t> Data;
};