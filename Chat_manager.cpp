#include <thread>
#include <chrono>
#include <unistd.h>
#include <netdb.h>

#include "Chat_manager.hpp"

void ClientChatManager::Send(SharedQueue<std::string>* pQueue, int SocketFd)
{
    using namespace std::chrono_literals;
    
    std::string s;
    size_t size;

    while(true)
    {
        std::this_thread::sleep_for(100ms);
        
        s = pQueue->Pop();
        size = s.size();

        if(write(SocketFd, &size, sizeof(size)) == -1)
            return;
        if(write(SocketFd, s.data(), s.size()) == -1)
            return;
    }
}

void ClientChatManager::Receive(SharedQueue<std::string>* pQueue, int SocketFd)
{
    using namespace std::chrono_literals;

    std::string s;
    size_t size;

    while(true)
    {
        std::this_thread::sleep_for(100ms);

        if(read(SocketFd, &size, sizeof(size)) == -1)
            return;
        s.resize(size);
        if(read(SocketFd, const_cast<char*>(s.data()), size) == -1)
            return;

        pQueue->Push(s);
    }
}

ClientChatManager::ClientChatManager(std::string IP, std::string Port, std::string Nick, SharedQueue<std::string>* pQueueIn, SharedQueue<std::string>* pQueueOut)
    : pQueueIn(pQueueIn), pQueueOut(pQueueOut)
{   
    addrinfo *resolved, hints={.ai_flags=0, .ai_family=AF_INET, .ai_socktype=SOCK_STREAM};
    int res = getaddrinfo(IP.c_str(), Port.c_str(), &hints, &resolved);
	if(res or !resolved)
        throw std::runtime_error("Could not resolve address.");

    SocketFd = socket(resolved->ai_family, resolved->ai_socktype, 0);
    if(SocketFd == -1)
        throw std::runtime_error("Socket creation failed.");   

    if(connect(SocketFd, resolved->ai_addr, resolved->ai_addrlen) == -1)
        throw std::runtime_error("Connection failed.");

    freeaddrinfo(resolved);
}

void ClientChatManager::Run()
{
    std::thread TSend(Send, pQueueIn, SocketFd);
    std::thread TReceive(Receive, pQueueOut, SocketFd);

    TSend.join();
    TReceive.join();

    close(SocketFd);
}