#include "net/NetManager.hpp"

int main()
{
    int a = 0;
    if(a == 0)
    {
        NetManager Manager("8303");
        Manager.Run();
    }
    else
    {
        NetManager Manager("8304");
        std::thread T([&]{Manager.Run();});
        Manager.Connect("192.168.0.100", "8303");
        
        int conn;
        while(true)
        {
            conn = Manager.GetSocketNew();
            if(conn != -1)
                break;
        }
        
        Manager.Push(new GamePacket{GamePacketType::CLIENT_INFO, conn, std::vector<uint8_t>{1,2,3,4}});
        
        while(true)
        {
            
        }
    }
    

    return 0;
}