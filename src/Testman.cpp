#include "net/NetManager.hpp"

int main()
{
    NetManager Manager("55055");
    Manager.Run();

    return 0;
}