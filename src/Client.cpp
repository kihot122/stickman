#include <iostream>
#include <string>
#include <thread>

#include "aux/Messenger.hpp"
#include "net/NetManager.hpp"
#include "render/Renderer.hpp"

void Unpack_ServerModelCreateBulk(GamePacket *Packet, Renderer &Backend)
{
	for (auto iter = Packet->Data.begin(); iter != Packet->Data.end();)
	{
		uint16_t ModelID = *reinterpret_cast<uint16_t *>(&(*iter));
		iter += sizeof(uint16_t);

		uint16_t VertexCount = *reinterpret_cast<uint16_t *>(&(*iter));
		iter += sizeof(uint16_t);

		std::vector<Renderer::Vertex> V;
		for (uint16_t i = 0; i < VertexCount; i++)
		{
			Renderer::Vertex Vert = *reinterpret_cast<Renderer::Vertex *>(&(*iter));
			V.push_back(Vert);
			iter += sizeof(Renderer::Vertex);
		}

		uint16_t IndexCount = *reinterpret_cast<uint16_t *>(&(*iter));
		iter += sizeof(uint16_t);

		std::vector<uint16_t> I;
		for (uint16_t i = 0; i < IndexCount; i++)
		{
			I.push_back(*reinterpret_cast<uint16_t *>(&(*iter)));
			iter += sizeof(uint16_t);
		}

		Backend.RenderModelCreate(ModelID, Renderer::RenderModel(V, I));
	}
}

void Unpack_ServerTargetCreateBulk(GamePacket *Packet, Renderer &Backend)
{
	for (auto iter = Packet->Data.begin(); iter != Packet->Data.end();)
	{
		uint16_t ModelID = *reinterpret_cast<uint16_t *>(&(*iter));
		iter += sizeof(uint16_t);

		uint16_t TargetID = *reinterpret_cast<uint16_t *>(&(*iter));
		iter += sizeof(uint16_t);

		Backend.RenderTargetCreate(TargetID, ModelID);
	}
}

void Unpack_ServerTargetUpdateBulk(GamePacket *Packet, Renderer &Backend)
{
	for (auto iter = Packet->Data.begin(); iter != Packet->Data.end();)
	{
		uint16_t TargetID = *reinterpret_cast<uint16_t *>(&(*iter));
		iter += sizeof(uint16_t);

		glm::mat4 Matrix = *reinterpret_cast<glm::mat4 *>(&(*iter));
		iter += sizeof(glm::mat4);

		Backend.RenderTargetUpdate(TargetID, Matrix);
	}
}

void Unpack_ServerTargetDeleteBulk(GamePacket *Packet, Renderer &Backend)
{
	for (auto iter = Packet->Data.begin(); iter != Packet->Data.end();)
	{
		uint16_t TargetID = *reinterpret_cast<uint16_t *>(&(*iter));
		iter += sizeof(uint16_t);

		Backend.RenderTargetDelete(TargetID);
	}
}

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
	Renderer Rend;

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
		Rend.ViewTransformUpdate(glm::lookAt(glm::vec3(0.0f, 0.0f, 12.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

		while (!glfwWindowShouldClose(Rend.GetWindowHandle()))
		{
			for (auto &Packet : Manager.Pull())
			{
				if (Packet->Type == GamePacketType::SERVER_ECHO_MESSAGE)
					Message(std::string(Packet->Data.begin(), Packet->Data.end()), MessageSource::CHAT, MessageSeverity::INFO);

				if (Packet->Type == GamePacketType::SERVER_MODEL_CREATE_BULK)
				{
					Message("Model create bulk", MessageSource::CLIENT, MessageSeverity::INFO);
					Unpack_ServerModelCreateBulk(Packet, Rend);
				}

				if (Packet->Type == GamePacketType::SERVER_TARGET_CREATE_BULK)
				{
					Message("Target create bulk", MessageSource::CLIENT, MessageSeverity::INFO);
					Unpack_ServerTargetCreateBulk(Packet, Rend);
				}

				if (Packet->Type == GamePacketType::SERVER_TARGET_UPDATE_BULK)
				{
					Message("Target update bulk", MessageSource::CLIENT, MessageSeverity::INFO);
					Unpack_ServerTargetUpdateBulk(Packet, Rend);
				}

				if (Packet->Type == GamePacketType::SERVER_TARGET_REMOVE_BULK)
				{
					Message("Target remove bulk", MessageSource::CLIENT, MessageSeverity::INFO);
					Unpack_ServerTargetDeleteBulk(Packet, Rend);
				}

				delete Packet;
			}

			glfwPollEvents();
			Rend.Draw();
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	Manager.Disconnect(ServerFd);
	exit(0);
}