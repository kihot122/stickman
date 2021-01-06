#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <set>

#include "aux/Messenger.hpp"
#include "game/Entity.hpp"
#include "net/NetManager.hpp"
#include "box2d/box2d.h"

const int64_t TickTime = std::pow(10, 9) / 64;
const float TickStep = 1.0f/64.0f;

int Unpack_ClientMove(GamePacket *Packet)
{
    return static_cast<int>(*reinterpret_cast<uint16_t *>(&(*Packet->Data.begin())));
}

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

GamePacket *Pack_ServerModelDeleteBulk(std::vector<Entity *> &pEntities, int DestinationFd)
{
    std::vector<uint8_t> PacketData;

    for (auto pEntity : pEntities)
    {
        uint16_t ModelID = pEntity->GetModelID();
        uint8_t *ParsedModelID = reinterpret_cast<uint8_t *>(&ModelID);

        PacketData.insert(PacketData.end(), ParsedModelID, ParsedModelID + sizeof(uint16_t));
    }

    return new GamePacket{GamePacketType::SERVER_MODEL_REMOVE_BULK, DestinationFd, PacketData};
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

GamePacket *Pack_ServerViewUpdate(glm::mat4 Matrix, int DestinationFd)
{
    std::vector<uint8_t> PacketData;
    uint8_t *ParsedMatrix = reinterpret_cast<uint8_t *>(&Matrix);
    PacketData.insert(PacketData.end(), ParsedMatrix, ParsedMatrix + sizeof(glm::mat4));

    return new GamePacket{GamePacketType::SERVER_VIEW_UPDATE, DestinationFd, PacketData};
}

GamePacket *Pack_ServerEchoMessage(GamePacket *Msg, std::map<int, std::string> &NicknameMapping, int DestinationFd)
{
    std::vector<uint8_t> PacketData;

    PacketData.insert(PacketData.end(), NicknameMapping[Msg->Socket].begin(), NicknameMapping[Msg->Socket].end());
    PacketData.push_back(':');
    PacketData.insert(PacketData.end(), Msg->Data.begin(), Msg->Data.end());

    return new GamePacket{GamePacketType::SERVER_ECHO_MESSAGE, DestinationFd, PacketData};
}

int main()
{
    std::vector<Entity *> TickingEntities;
    std::vector<Entity *> UpdateEntities;
    std::vector<Entity *> CreateEntities;
    std::vector<Entity *> DeleteEntities;
    b2Vec2 gravity(0.0f, -9.81f);
    b2World *world = new b2World(gravity);
    CreateEntities.push_back(new EntityWall(1, 1, 20.0f, 1.0f, 0.0f, -40.0f, world, false));
    CreateEntities.push_back(new EntityWall(2, 2, 1.0f, 40.0f, -54.0f, 0.0f, world, false));
    CreateEntities.push_back(new EntityWall(3, 3, 1.0f, 40.0f, 54.0f, 0.0f, world, false));
    CreateEntities.push_back(new EntityWall(4, 4, 50.0f, 1.0f, 0.0f, 40.0f, world, false));
    CreateEntities.push_back(new EntityWall(5, 5, 10.0f, 1.0f, 20.0f, 10.0f, world, false));
    CreateEntities.push_back(new EntityWall(6, 6, 5.0f, 1.5f, -8.0f, 4.0f, world, false));
    CreateEntities.push_back(new EntityWall(7, 7, 15.0f, 1.0f, -15.0f, -30.0f, world, false));
    CreateEntities.push_back(new EntityWall(8, 8, 20.0f, 1.0f, -10.0f, -20.0f, world, false));

    NetManager Manager("8303");

    std::set<int> ConnectedPlayers;
    std::map<int, std::string> NicknameMapping;

    auto DiscreteTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - DiscreteTime).count() >= TickTime)
        {
            DiscreteTime += std::chrono::nanoseconds(TickTime);

            std::vector<GamePacket *> Packets = Manager.Pull();
            for (auto &Packet : Packets)
            {
                //Message("Packet type: " + std::string(magic_enum::enum_name(Packet->Type)), MessageSource::SERVER, MessageSeverity::INFO);

                if (Packet->Type == GamePacketType::CLIENT_MESSAGE)
                {
                    for (int Player : ConnectedPlayers)
                        Manager.Push(Pack_ServerEchoMessage(Packet, NicknameMapping, Player));
                }

                if (Packet->Type == GamePacketType::CLIENT_NICKNAME)
                {
                    NicknameMapping.emplace(Packet->Socket, std::string(Packet->Data.begin(), Packet->Data.end()));
                }

                if (Packet->Type == GamePacketType::CLIENT_MOVE)
                {
                    for(auto pEntity : UpdateEntities)
                    {
                        if(pEntity->GetType() == EntityType::PLAYER)
                        {   
                            if(reinterpret_cast<EntityPlayer*>(pEntity)->GetSocket() == Packet->Socket)
                            {
                                reinterpret_cast<EntityPlayer*>(pEntity)->SetAction(Unpack_ClientMove(Packet));
                            } 
                        }
                    }
                }

                delete Packet;
            }
            //world.step
            world->Step(TickStep, 6, 2);

            for (auto pEntity : CreateEntities)
            {
                if (pEntity->IsDirty())
                    UpdateEntities.push_back(pEntity);

                for (int Player : ConnectedPlayers)
                {
                    Manager.Push(Pack_ServerModelCreateBulk(CreateEntities, Player));
                    Manager.Push(Pack_ServerTargetCreateBulk(CreateEntities, Player));
                    Manager.Push(Pack_ServerTargetUpdateBulk(CreateEntities, Player));
                }
            }

            TickingEntities.insert(TickingEntities.end(), CreateEntities.begin(), CreateEntities.end());
            CreateEntities.clear();

            for (auto pEntity : TickingEntities)
                pEntity->Tick();

            if (UpdateEntities.size())
                for (int Player : ConnectedPlayers)
                    Manager.Push(Pack_ServerTargetUpdateBulk(UpdateEntities, Player));

            for (auto iter = TickingEntities.begin(); iter != TickingEntities.end();)
            {
                if ((*iter)->IsDone())
                {
                    DeleteEntities.push_back(*iter);
                    iter = TickingEntities.erase(iter);
                    continue;
                }
                iter++;
            }

            for (auto iter = UpdateEntities.begin(); iter != UpdateEntities.end();)
            {
                if ((*iter)->IsDone())
                {
                    iter = UpdateEntities.erase(iter);
                    continue;
                }
                iter++;
            }

            for (auto pEntity : DeleteEntities)
            {
                for (int Player : ConnectedPlayers)
                    Manager.Push(Pack_ServerTargetRemoveBulk(DeleteEntities, Player));

                delete pEntity;
            }

            DeleteEntities.clear();

            int NewPlayer = Manager.GetSocketNew();
            if (NewPlayer != -1)
            {
                ConnectedPlayers.emplace(NewPlayer);
                Message("Player connected", MessageSource::SERVER, MessageSeverity::INFO);

                Manager.Push(Pack_ServerModelCreateBulk(TickingEntities, NewPlayer));
                Manager.Push(Pack_ServerTargetCreateBulk(TickingEntities, NewPlayer));
                Manager.Push(Pack_ServerTargetUpdateBulk(TickingEntities, NewPlayer));

                //create entity -nowy gracz
                CreateEntities.push_back(new EntityPlayer(9, 9, 1.0f, 2.0f, 20.0f, 22.0f, world, true, NewPlayer));

            }

            int DelPlayer = Manager.GetSocketDel();
            if (DelPlayer != -1)
            {
                ConnectedPlayers.erase(DelPlayer);
                NicknameMapping.erase(DelPlayer);

                Message("Player disconnected", MessageSource::SERVER, MessageSeverity::INFO);
            }
        }
    }
}