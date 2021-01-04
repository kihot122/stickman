#include <iostream>
#include <string>
#include <thread>

#include "aux/Messenger.hpp"
#include "net/NetManager.hpp"
#include "render/Renderer.hpp"

void Chat(NetManager *Manager, int ServerFd)
{
	std::string Msg;
	while (true)
	{
		std::getline(std::cin, Msg);
		Manager->Push(new GamePacket{GamePacketType::CLIENT_MESSAGE, ServerFd, std::vector<uint8_t>(Msg.begin(), Msg.end())});
	}
}

int main()
{
	Renderer app;

	const std::vector<Renderer::Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0};

	std::string Port;
	int a = 0;
	if (a == 0)
		Port = "8304";
	else
		Port = "8305";

	NetManager Manager(Port);
	Manager.Connect("192.168.0.104", "8303");

	int ServerFd = -1;
	while (ServerFd == -1)
		ServerFd = Manager.GetSocketNew();

	std::thread ChatThread(Chat, &Manager, ServerFd);

	try
	{
		app.RenderModelCreate(1, Renderer::RenderModel(vertices, indices));

		app.RenderTargetCreate(1, 1);
		app.RenderTargetUpdate(1, glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		app.RenderTargetCreate(2, 1);
		app.RenderTargetUpdate(2, glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -0.5f, 0.0f)));

		app.ViewTransformUpdate(glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		while (!glfwWindowShouldClose(app.GetWindowHandle()))
		{
			for (auto &Packet : Manager.Pull())
			{
				if (Packet->Type == GamePacketType::SERVER_ECHO)
					Message(std::string(Packet->Data.begin(), Packet->Data.end()), MessageSource::CHAT, MessageSeverity::INFO);

				delete Packet;
			}

			glfwPollEvents();
			app.Draw();
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	exit(0);
}