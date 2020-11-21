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

    size_t size = Nick.size();
    write(SocketFd, &size, sizeof(size_t));
    write(SocketFd, Nick.data(), size);
}

void ClientChatManager::Run()
{
    std::thread TSend(Send, pQueueIn, SocketFd);
    std::thread TReceive(Receive, pQueueOut, SocketFd);

    TSend.join();
    TReceive.join();

    close(SocketFd);
}

ServerChatManager::ServerChatManager(char* port_number)
{
    port = readPort(port_number);
}

void ServerChatManager::ctrl_c(int)
{
    for(int clientFd : clientFds)
		close(clientFd);
	close(servFd);
	printf("Closing server\n");
	exit(0);
}

int ServerChatManager::Run()
{
    servFd = socket(AF_INET, SOCK_STREAM, 0);
	if(servFd == -1) 
		error(1, errno, "socket failed");
	
	//signal(SIGINT, ctrl_c);
	//signal(SIGPIPE, SIG_IGN);
	
	setReuseAddr(servFd);
	
	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	int res = bind(servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");
	
	res = listen(servFd, 1);
	if(res) 
		error(1, errno, "listen failed");
	

	//int num = 0;
	//std::thread threads[100];
	std::vector<std::thread*> threads;
	//int w = 0;
	while(true){
		sockaddr_in clientAddr{0};
		socklen_t clientAddrSize = sizeof(clientAddr);
		
		auto clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
		if(clientFd == -1) 
			error(1, errno, "accept failed");
		
		clientFds.insert(clientFd);
		size_t nick_size;
		std::string s;
		mtx.lock();
		if(read(clientFd, &nick_size, sizeof(size_t)) == -1) 
			return -1;
		mtx.unlock();
		s.resize(nick_size);
		mtx.lock();
		if(read(clientFd, const_cast<char*>(s.data()), nick_size) == -1)
			return -1;
		mtx.unlock();
		nick.insert_or_assign(clientFd, s);
		
		printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);
		
		threads.push_back(new std::thread(receive, clientFd));
		//threads[w++] = std::thread(receive, clientFd);	
	}
	for(int clientFd : clientFds)
		close(clientFd);
	
	while(true)
	{
		for(auto iter = threads.begin(); iter != threads.end(); iter++)
			if((*iter)->joinable())
			{
				(*iter)->join();
				delete (*iter);
				threads.erase(iter);
				break;
			}
		//std::this_thread::sleep_for();
	}
	
	
	/*for(int i = 0; i < num; i++)
		threads[i].join();*/
	for(auto &t : threads)
	{
		t->join();
	}
	return 0;
}



void ServerChatManager::sendToAllBut(int fd, char * buffer, int count)
{
    int res;
	std::string temp = nick[fd]+ ": " + std::string(buffer);
	count = temp.size();

	decltype(clientFds) bad;
	
	for(int clientFd : clientFds){
		if(clientFd == fd) continue;
		mtx.lock();
		if(write(clientFd, &fd, sizeof(int)) == -1)
			return;
		mtx.unlock();
		mtx.lock();
		res = write(clientFd, temp.data(), count);
		mtx.unlock();
		if(res!=count)
			bad.insert(clientFd);
	}
	for(int clientFd : bad){
		printf("removing %d\n", clientFd);
		clientFds.erase(clientFd);
		close(clientFd);
	}
}

uint16_t ServerChatManager::readPort(char * txt)
{
    char * ptr;
	auto port = strtol(txt, &ptr, 10);
	if(*ptr!=0 || port<1 || (port>((1<<16)-1))) 
		error(1,0,"illegal argument %s", txt);
	return port;
}
void ServerChatManager::setReuseAddr(int sock)
{
    const int one = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	if(res) 
		error(1,errno, "setsockopt failed");
}
void ServerChatManager::receive(int clientFd)
{
    while(1)
	{
		size_t buff_size;
		int tmp = read(clientFd, &buff_size, sizeof(size_t));
		char buffer[buff_size];
		int count = read(clientFd, buffer, buff_size);
		if(count < 1) {
			printf("removing %d\n", clientFd);
			clientFds.erase(clientFd);
			close(clientFd);
			break;
		}
		else {
			sendToAllBut(clientFd, buffer, count);
		}
	}
}