#include <string>
#include <queue>
#include <mutex>
#include <cstdlib>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h> 
#include <thread>
#include <unordered_set>
#include <signal.h>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <map>

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

class ServerChatManager
{
private:
    std::mutex mtx;
    int servFd;
    std::map <int, std::string> nick;
    std::unordered_set<int> clientFds;
    long int port;

    void sendToAllBut(int fd, char * buffer, int count);
    uint16_t readPort(char * txt);
    void setReuseAddr(int sock);
    void receive(int clientFd);
public:
    ServerChatManager(char* port_number);
    int Run();  
    void ctrl_c(int);

};