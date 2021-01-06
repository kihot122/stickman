#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <set>

#include "aux/Messenger.hpp"
#include "game/Entity.hpp"
#include "net/NetManager.hpp"

const int64_t TickTime = std::pow(10, 9) / 64;

GamePacket *Pack_ServerModelCreateBulk(std::vector<Entity *> &pEntities, int DestinationFd)
{
    std::vector<uint8_t> PacketData;
    std::set<uint16_t> CheckedModelID;

    for (auto pEntity : pEntities)
    {
        uint16_t ID = pEntity->GetModelID();
        if (CheckedModelID.find(ID) != CheckedModelID.end())
            continue;
        else
            CheckedModelID.emplace(ID);

        uint8_t *ParsedID = reinterpret_cast<uint8_t *>(&ID);

        std::vector<uint8_t> T(sizeof(uint16_t));
        std::memcpy(T.data(), ParsedID, sizeof(uint16_t));

        PacketData.insert(PacketData.end(), T.begin(), T.end());

        uint16_t VertexCount = static_cast<uint16_t>(pEntity->ModelCreateVertices().size());
        uint8_t *ParsedVertexCount = reinterpret_cast<uint8_t *>(&VertexCount);

        std::memcpy(T.data(), ParsedVertexCount, sizeof(uint16_t));

        PacketData.insert(PacketData.end(), T.begin(), T.end());

        for (auto &&Vertex : pEntity->ModelCreateVertices())
        {
            std::vector<uint8_t> V(sizeof(Vertex));
            std::memcpy(V.data(), reinterpret_cast<uint8_t *>(&Vertex), sizeof(Vertex));

            PacketData.insert(PacketData.end(), V.begin(), V.end());
        }

        uint16_t IndexCount = static_cast<uint16_t>(pEntity->ModelCreateIndices().size());
        uint8_t *ParsedIndexCount = reinterpret_cast<uint8_t *>(&IndexCount);

        std::memcpy(T.data(), ParsedIndexCount, sizeof(uint16_t));

        PacketData.insert(PacketData.end(), T.begin(), T.end());

        for (auto &&Index : pEntity->ModelCreateIndices())
        {
            std::vector<uint8_t> I(sizeof(uint16_t));
            std::memcpy(I.data(), reinterpret_cast<uint8_t *>(&Index), sizeof(uint16_t));

            PacketData.insert(PacketData.end(), I.begin(), I.end());
        }
    }

    return new GamePacket{GamePacketType::SERVER_MODEL_CREATE_BULK, DestinationFd, PacketData};
}

GamePacket *Pack_ServerTargetCreateBulk(std::vector<Entity *> &pEntities, int DestinationFd)
{
    std::vector<uint8_t> PacketData;

    for (auto pEntity : pEntities)
    {
        uint16_t ModelID = pEntity->GetModelID();
        uint16_t TargetID = pEntity->GetTargetID();

        uint8_t *ParsedModelID = reinterpret_cast<uint8_t *>(&ModelID);
        uint8_t *ParsedTargetID = reinterpret_cast<uint8_t *>(&TargetID);

        PacketData.insert(PacketData.end(), ParsedModelID, ParsedModelID + sizeof(uint16_t));
        PacketData.insert(PacketData.end(), ParsedTargetID, ParsedTargetID + sizeof(uint16_t));
    }

    return new GamePacket{GamePacketType::SERVER_TARGET_CREATE_BULK, DestinationFd, PacketData};
}

GamePacket *Pack_ServerTargetUpdateBulk(std::vector<Entity *> &pEntities, int DestinationFd)
{
    std::vector<uint8_t> PacketData;

    for (auto pEntity : pEntities)
    {
        uint16_t TargetID = pEntity->GetTargetID();
        uint8_t *ParsedTargetID = reinterpret_cast<uint8_t *>(&TargetID);

        glm::mat4 Matrix = pEntity->TargetUpdate();
        uint8_t *ParsedMatrix = reinterpret_cast<uint8_t *>(&Matrix);

        PacketData.insert(PacketData.end(), ParsedTargetID, ParsedTargetID + sizeof(uint16_t));
        PacketData.insert(PacketData.end(), ParsedMatrix, ParsedMatrix + sizeof(glm::mat4));
    }

    return new GamePacket{GamePacketType::SERVER_TARGET_UPDATE_BULK, DestinationFd, PacketData};
}

GamePacket *Pack_ServerTargetRemoveBulk(std::vector<Entity *> &pEntities, int DestinationFd)
{
    std::vector<uint8_t> PacketData;

    for (auto pEntity : pEntities)
    {
        uint16_t TargetID = pEntity->GetTargetID();
        uint8_t *ParsedTargetID = reinterpret_cast<uint8_t *>(&TargetID);

        PacketData.insert(PacketData.end(), ParsedTargetID, ParsedTargetID + sizeof(uint16_t));
    }

    return new GamePacket{GamePacketType::SERVER_TARGET_REMOVE_BULK, DestinationFd, PacketData};
}

int main()
{
    std::vector<Entity *> vEnt;
    vEnt.push_back(new EntityWall());
    // vEnt.push_back(new EntityWall());

    NetManager Manager("8303");
    std::set<int> ConnectedPlayers;
    auto DiscreteTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - DiscreteTime).count() >= TickTime)
        {
            DiscreteTime += std::chrono::nanoseconds(TickTime);
            // std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - DiscreteTime).count() << std::endl;

            int NewPlayer = Manager.GetSocketNew();
            if (NewPlayer != -1)
            {
                ConnectedPlayers.emplace(NewPlayer);
                Message("Player connected", MessageSource::SERVER, MessageSeverity::INFO);

                Manager.Push(Pack_ServerModelCreateBulk(vEnt, NewPlayer));
                Manager.Push(Pack_ServerTargetCreateBulk(vEnt, NewPlayer));
                Manager.Push(Pack_ServerTargetUpdateBulk(vEnt, NewPlayer));
            }

            int DelPlayer = Manager.GetSocketDel();
            if (DelPlayer != -1)
            {
                ConnectedPlayers.erase(DelPlayer);
                Message("Player disconnected", MessageSource::SERVER, MessageSeverity::INFO);
            }

            std::vector<GamePacket *> Packets = Manager.Pull();
            for (auto &Packet : Packets)
            {
                Message("Packet type: " + std::string(magic_enum::enum_name(Packet->Type)), MessageSource::SERVER, MessageSeverity::INFO);

                if (Packet->Type == GamePacketType::CLIENT_MESSAGE)
                {
                    for (int Player : ConnectedPlayers)
                        Manager.Push(new GamePacket{GamePacketType::SERVER_ECHO_MESSAGE, Player, Packet->Data});

                    Manager.Push(Pack_ServerTargetRemoveBulk(vEnt, Packet->Socket));
                }

                delete Packet;
            }
        }
    }
}