#include <cstdint>
#include <string>

enum class GameCommandType : uint8_t
{
    NET_CONN,
    NET_DISCONN,
};

struct GameCommand
{
    GameCommandType Type;
    virtual ~GameCommand() {}
};

struct GameCommandNetConnect : public GameCommand
{
    std::string Address;
    std::string Port;

    GameCommandNetConnect(std::string Address, std::string Port) : Address(Address), Port(Port)
    {
        Type = GameCommandType::NET_CONN;
    }
};