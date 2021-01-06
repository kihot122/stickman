#pragma once

#include <cstdint>
#include <string>
#include <vector>

enum class GamePacketType : uint8_t
{
    CLIENT_MESSAGE,
    CLIENT_NICKNAME,
    CLIENT_MOVE,
    SERVER_ECHO_MESSAGE,
    SERVER_MODEL_CREATE_BULK,
    SERVER_MODEL_REMOVE_BULK,
    SERVER_TARGET_CREATE_BULK,
    SERVER_TARGET_UPDATE_BULK,
    SERVER_TARGET_REMOVE_BULK,
    SERVER_VIEW_UPDATE,
};

struct GamePacket
{
    GamePacketType Type;
    int Socket;
    std::vector<uint8_t> Data;
};