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
#include <mutex>
#include <string>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <cmath>

using namespace std;

mutex mtx;
int servFd;

unordered_set<int> clientFds;
void ctrl_c(int);
void sendToAllBut(int fd, char * buffer, int count);
uint16_t readPort(char * txt);
void setReuseAddr(int sock);
void receive(int clientFd);

int main(int argc, char ** argv){

	if(argc != 2) error(1, 0, "Need 1 arg (port)");
	auto port = readPort(argv[1]);
	
	servFd = socket(AF_INET, SOCK_STREAM, 0);
	if(servFd == -1) 
		error(1, errno, "socket failed");
	
	signal(SIGINT, ctrl_c);
	signal(SIGPIPE, SIG_IGN);
	
	setReuseAddr(servFd);
	
	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	int res = bind(servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");
	
	res = listen(servFd, 1);
	if(res) 
		error(1, errno, "listen failed");
	
	int num = 0;
	std::thread threads[100];
	int w = 0;
	while(true){
		sockaddr_in clientAddr{0};
		socklen_t clientAddrSize = sizeof(clientAddr);
		
		auto clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
		if(clientFd == -1) 
			error(1, errno, "accept failed");
		
		clientFds.insert(clientFd);
		
		printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);
		

		threads[w++] = std::thread(receive, clientFd);	
	}
	for(int clientFd : clientFds)
		close(clientFd);
	for(int i = 0; i < num; i++)
		threads[i].join();
}

uint16_t readPort(char * txt){
	char * ptr;
	auto port = strtol(txt, &ptr, 10);
	if(*ptr!=0 || port<1 || (port>((1<<16)-1))) 
		error(1,0,"illegal argument %s", txt);
	return port;
}

void setReuseAddr(int sock){
	const int one = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	if(res) 
		error(1,errno, "setsockopt failed");
}

void ctrl_c(int){
	for(int clientFd : clientFds)
		close(clientFd);
	close(servFd);
	printf("Closing server\n");
	exit(0);
}

void sendToAllBut(int fd, char* buffer, int count){
	int res;
	string s = to_string(fd) + ": " + buffer + "\n";
	char temp[s.length()];
	strcpy(temp, s.c_str());
	count += (ceil(fd/10) + 3);

	decltype(clientFds) bad;
	
	for(int clientFd : clientFds){
		if(clientFd == fd) continue;
		int name = write(clientFd, &fd, sizeof(int));
		mtx.lock();
		res = write(clientFd, temp, count);
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

void receive(int clientFd)
{
	while(1)
	{
		char buffer[255];
		int count = read(clientFd, buffer, 255);
		printf("tak");
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