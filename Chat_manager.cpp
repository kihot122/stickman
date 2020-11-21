#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <netdb.h>

#include "Chat_manager.hpp"

void ClientChatManager::Send(SharedQueue<std::string> *pQueue, int SocketFd)
{
	using namespace std::chrono_literals;

	std::string s;
	size_t size;

	while (true)
	{
		std::this_thread::sleep_for(100ms);

		s = pQueue->Pop();
		size = s.size();

		if (write(SocketFd, &size, sizeof(size)) == -1)
			return;
		if (write(SocketFd, s.data(), s.size()) == -1)
			return;
	}
}

void ClientChatManager::Receive(SharedQueue<std::string> *pQueue, int SocketFd)
{
	using namespace std::chrono_literals;

	std::string s;
	size_t size;

	while (true)
	{
		std::this_thread::sleep_for(100ms);

		if (read(SocketFd, &size, sizeof(size)) == -1)
			return;
		s.resize(size);
		if (read(SocketFd, const_cast<char *>(s.data()), size) == -1)
			return;

		pQueue->Push(s);
	}
}

ClientChatManager::ClientChatManager(std::string IP, std::string Port, std::string Nick, SharedQueue<std::string> *pQueueIn, SharedQueue<std::string> *pQueueOut)
	: pQueueIn(pQueueIn), pQueueOut(pQueueOut)
{
	addrinfo *resolved, hints{};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int res = getaddrinfo(IP.c_str(), Port.c_str(), &hints, &resolved);
	if (res or !resolved)
		throw std::runtime_error("Could not resolve address.");

	SocketFd = socket(resolved->ai_family, resolved->ai_socktype, 0);
	if (SocketFd == -1)
		throw std::runtime_error("Socket creation failed.");

	if (connect(SocketFd, resolved->ai_addr, resolved->ai_addrlen) == -1)
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

void ServerChatManager::Echo(int SenderFd, std::set<int> &ClientFd, std::map<int, std::string> &Nick, std::mutex &M, std::string &Message)
{
	M.lock();

	for (auto &Recipient : ClientFd)
	{
		std::string Msg = Nick[SenderFd] + " : " + Message;
		size_t Size = Msg.size();

		write(Recipient, &Size, sizeof(size_t));
		write(Recipient, Msg.data(), Msg.size());
	}

	M.unlock();
}

void ServerChatManager::Listen(int SenderFd, std::set<int> &ClientFd, std::map<int, std::string> &Nick, std::mutex &M, int &Flag)
{
	std::string Msg;
	size_t Size;

	while (true)
	{
		if (read(SenderFd, &Size, sizeof(size_t)) == -1)
			break;
		Msg.resize(Size);
		if (read(SenderFd, const_cast<char *>(Msg.data()), Msg.size()) == -1)
			break;

		Echo(SenderFd, ClientFd, Nick, M, Msg);
	}

	M.lock();
	ClientFd.erase(SenderFd);
	Nick.erase(SenderFd);
	M.unlock();

	Flag = 1;
}

ServerChatManager::ServerChatManager(std::string Port)
{
	SocketFd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in ServerAddress{};
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(std::stoi(Port));
	ServerAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(SocketFd, reinterpret_cast<sockaddr *>(&ServerAddress), sizeof(ServerAddress)) != 0)
		throw std::runtime_error("Unable to bind port.");

	if (listen(SocketFd, 16) != 0)
		throw std::runtime_error("Unable to listen on port.");
}

void ServerChatManager::Run()
{
	std::vector<std::thread *> Workers;
	std::vector<int> Control;
	std::string Msg;
	size_t Size;

	while (true)
	{
		sockaddr_in ClientAddress{};
		socklen_t ClientAddressSize;

		int Result = accept(SocketFd, reinterpret_cast<sockaddr *>(&ClientAddress), &ClientAddressSize);
		if (Result == -1)
		{
			std::cerr << "Error accepting new connection." << std::endl;
			continue;
		}

		if (read(Result, &Size, sizeof(size_t)) == -1)
		{
			std::cerr << "Error reading from new connection." << std::endl;
			continue;
		}

		Msg.resize(Size);

		if (read(Result, const_cast<char *>(Msg.data()), Msg.size()) == -1)
		{
			std::cerr << "Error reading from new connection." << std::endl;
			continue;
		}

		M.lock();
		ClientFd.insert(Result);
		Nick.insert(std::pair(Result, Msg));
		M.unlock();

		for (size_t i = 0; i < Control.size();)
		{
			if (Control[i])
			{
				Workers[i]->join();
				delete Workers[i];
				Workers.erase(Workers.begin() + i);
				Control.erase(Control.begin() + i);
				continue;
			}
			i++;
		}

		Control.push_back(false);
		Workers.push_back(new std::thread(Listen, Result, std::ref(ClientFd), std::ref(Nick), std::ref(M), std::ref(Control.front())));
	}
}