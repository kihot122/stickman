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
        while(true)
        {
            int c = 'a';
        }
    }
    

    return 0;
}