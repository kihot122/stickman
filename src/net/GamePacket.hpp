#pragma once

#include <cstdint>

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
    uint32_t Length;
    uint8_t *Data;
};