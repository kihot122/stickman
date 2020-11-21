#include <string>
#include <set>
#include <map>

#include "SharedQueue.hpp"

class ClientChatManager
{
    int SocketFd;

    SharedQueue<std::string> *pQueueIn;
    SharedQueue<std::string> *pQueueOut;

    static void Send(SharedQueue<std::string> *pQueue, int SocketFd);
    static void Receive(SharedQueue<std::string> *pQueue, int SocketFd);

public:
    ClientChatManager(std::string IP, std::string Port, std::string Nick, SharedQueue<std::string> *pQueueIn, SharedQueue<std::string> *pQueueOut);

    void Run();
};

class ServerChatManager
{
    int SocketFd;

    std::set<int> ClientFd;
    std::map<int, std::string> Nick;
    std::mutex M;

    static void Echo(int SenderFd, std::set<int> &ClientFd, std::map<int, std::string> &Nick, std::mutex &M, std::string &Message);
    static void Listen(int SenderFd, std::set<int> &ClientFd, std::map<int, std::string> &Nick, std::mutex &M, int &Flag);

public:
    ServerChatManager(std::string Port);

    void Run();
};