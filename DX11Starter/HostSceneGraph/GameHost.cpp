#include "GameHost.h"
#include "DataNetworkStructs.h"
#include "RequestNetworkStructs.h"

GameHost::GameHost(ServerManager* server)
{
	m_inactive = true;
	m_serverPointer = server;
}

GameHost::~GameHost()
{
}

void GameHost::HostRecieveClient(Buffer& data, int clientId)
{
	ClientRequest request;
	request.Deserialize(data);
	
	switch (request.m_request)
	{
	case ClientRequestType::Join:
		m_clientQueue.Push(clientId);
		if (m_clientQueue.GetCount() >= PLAYERS_PER_SESSION && m_inactive)
		{
			StartGame();
		}
		break;
	case ClientRequestType::Confirm:

		break;
	case ClientRequestType::Quit:

		break;
	default:
		break;
	}
}

void GameHost::HostRecievePlayer(Buffer& data, int playerId)
{

}

void GameHost::StartGame()
{
	m_inactive = true;
	for (size_t i = 0; i < PLAYERS_PER_SESSION; i++)
	{
		int client;
		m_clientQueue.Pop(client);
		m_serverPointer->SetUser(client, i);
		Buffer* buffer = m_serverPointer->GetNextBufferClient(MessageType::GameRequest, client);
		
		// Prepare request
		HostRequest request;
		request.m_request = HostRequestType::Prepare;
		//request.m_time = ...

		GamePreparationRequest prepare;
		prepare.m_playerEntityId = i;
		prepare.m_scene = 0;

		request.Serialize(*buffer);
		prepare.Serialize(*buffer);

		m_serverPointer->SendToClient(client);
	}
}
