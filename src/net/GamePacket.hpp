#pragma once

#include <cstdint>
#include <string>
#include <vector>

enum class GamePacketType : uint8_t
{
    CLIENT_MESSAGE,
    SERVER_ECHO,
};

struct GamePacket
{
    GamePacketType Type;
    int Socket;
    std::vector<uint8_t> Data;
};