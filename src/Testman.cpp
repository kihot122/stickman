#include "net/NetManager.hpp"
#include <iostream>

int main()
{
    std::cout << "here" << std::endl;
    int a = 0;
    if (a == 0)
    {
        NetManager Manager("8303");
        while (true)
        {
            std::vector<GamePacket *> Packets = Manager.Pull();
            for (auto &Packet : Packets)
                std::cout << static_cast<int>(Packet->Type) << std::endl;
        }
    }
    else
    {
        NetManager Manager("8304");
        Manager.Connect("192.168.8.111", "8303");

        int conn;
        while (true)
        {
            conn = Manager.GetSocketNew();
            if (conn != -1)
                break;
        }

        Manager.Push(new GamePacket{GamePacketType::SERVER_INFO, conn, std::vector<uint8_t>{1, 2, 3, 4}});

        using namespace std::chrono_literals;

        std::this_thread::sleep_for(1s);

        Manager.Disconnect(conn);

        while (true)
        {
        }
    }

    return 0;
}