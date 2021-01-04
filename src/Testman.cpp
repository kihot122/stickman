#include "aux/Messenger.hpp"
#include "net/NetManager.hpp"

int main()
{
    NetManager Manager("8304");

    for (int i = 0; i < 2; i++)
    {
        Manager.Connect("192.168.0.104", "8303");

        int conn;
        while (true)
        {
            conn = Manager.GetSocketNew();
            if (conn != -1)
                break;
        }

        Message("Connected", MessageSource::TESTMAN, MessageSeverity::INFO);

        Manager.Push(new GamePacket{GamePacketType::CLIENT_MESSAGE, conn, std::vector<uint8_t>{1, 2, 3, 4}});

        while (true)
        {
            std::vector<GamePacket *> Packets = Manager.Pull();
            if (Packets.size())
            {
                Message("Got response " + std::string(magic_enum::enum_name(Packets[0]->Type)), MessageSource::TESTMAN, MessageSeverity::INFO);
                delete Packets[0];
                break;
            }
        }

        Manager.Disconnect(conn);

        while (Manager.GetSocketDel() == -1)
            ;
        Message("Disconnected", MessageSource::TESTMAN, MessageSeverity::INFO);
    }
    return 0;
}