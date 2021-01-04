#include <chrono>
#include <cmath>
#include <iostream>
#include <set>

#include "aux/Messenger.hpp"
#include "net/NetManager.hpp"

const int64_t TickTime = std::pow(10, 9) / 64;

int main()
{
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
                        Manager.Push(new GamePacket{GamePacketType::SERVER_ECHO, Player, Packet->Data});
                }

                delete Packet;
            }
        }
    }
}