#include <string>
#include <queue>
#include <mutex>

template<typename T>
class SharedQueue
{
    std::queue<T> Elements;
    std::mutex M;

    public:
    SharedQueue();

    void Push(T Item);
    T Pop();
};

template<typename T>
SharedQueue<T>::SharedQueue()
{
    M.unlock();
}

template<typename T>
void SharedQueue<T>::Push(T Item)
{
    M.lock();
    Elements.push(Item);
    M.unlock();
}

template<typename T>
T SharedQueue<T>::Pop()
{
    T Item;
    M.lock();
    Item = Elements.front();
    Elements.pop();
    M.unlock();
    return Item;
}

class ClientChatManager
{
    int SocketFd;
    
    SharedQueue<std::string>* pQueueIn;
    SharedQueue<std::string>* pQueueOut;
    
    static void Send(SharedQueue<std::string>* pQueue, int SocketFd);
    static void Receive(SharedQueue<std::string>* pQueue, int SocketFd);

    public:
    ClientChatManager(std::string IP, std::string Port, std::string Nick, SharedQueue<std::string>* pQueueIn, SharedQueue<std::string>* pQueueOut);

    void Run();
};

class ServerChatManager{};