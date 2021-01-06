#include <iostream>
#include <string>
#include <thread>

#include "aux/Messenger.hpp"
#include "net/NetManager.hpp"
#include "render/Renderer.hpp"

enum class NetState
{
	NET_INIT,
	NET_GAME,
};

GamePacket *Pack_ClientMove(int Move, int DestinationFd)
{
	std::vector<uint8_t> PacketData;
	uint8_t *KeyID = reinterpret_cast<uint8_t *>(&Move);
	PacketData.insert(PacketData.end(), KeyID, KeyID + sizeof(uint16_t));

	return new GamePacket{GamePacketType::CLIENT_MOVE, DestinationFd, PacketData};
}

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

void Unpack_ServerModelRemoveBulk(GamePacket *Packet, Renderer &Backend)
{
	for (auto iter = Packet->Data.begin(); iter != Packet->Data.end();)
	{
		uint16_t ModelID = *reinterpret_cast<uint16_t *>(&(*iter));
		iter += sizeof(uint16_t);

		Backend.RenderModelDelete(ModelID);
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

void Unpack_ServerViewUpdate(GamePacket *Packet, Renderer &Backend)
{
	glm::mat4 Matrix = *reinterpret_cast<glm::mat4 *>(&(*Packet->Data.begin()));
	Backend.ViewTransformUpdate(Matrix);
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

struct Helper
{
	NetManager *pManager;
	int *pServerFd;
} * pHelp;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	pHelp->pManager->Push(Pack_ClientMove(key, *pHelp->pServerFd));
}

int main(int argc, char **argv)
{
	if (argc != 5)
		exit(1);

	Renderer Rend;

	std::string IP(argv[1]);
	std::string DstPort(argv[2]);
	std::string SrcPort(argv[3]);
	std::string Nick(argv[4]);

	NetManager Manager(SrcPort);
	Manager.Connect(IP, DstPort);

	NetState State = NetState::NET_INIT;

	int ServerFd = -1;
	while (ServerFd == -1)
		ServerFd = Manager.GetSocketNew();

	std::thread ChatThread(Chat, &Manager, ServerFd);

	pHelp = new Helper{&Manager, &ServerFd};
	glfwSetKeyCallback(Rend.GetWindowHandle(), KeyCallback);

	try
	{
		Rend.ViewTransformUpdate(glm::lookAt(glm::vec3(0.0f, 0.0f, 12.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

		while (!glfwWindowShouldClose(Rend.GetWindowHandle()))
		{
			if (State == NetState::NET_INIT)
			{
				Manager.Push(new GamePacket{GamePacketType::CLIENT_NICKNAME, ServerFd, std::vector<uint8_t>(Nick.begin(), Nick.end())});
				State = NetState::NET_GAME;
			}

			if (State == NetState::NET_GAME)
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

					if (Packet->Type == GamePacketType::SERVER_MODEL_REMOVE_BULK)
					{
						Message("Model remove bulk", MessageSource::CLIENT, MessageSeverity::INFO);
						Unpack_ServerModelRemoveBulk(Packet, Rend);
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

					if (Packet->Type == GamePacketType::SERVER_VIEW_UPDATE)
					{
						Message("View update", MessageSource::CLIENT, MessageSeverity::INFO);
						Unpack_ServerViewUpdate(Packet, Rend);
					}

					delete Packet;
				}
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