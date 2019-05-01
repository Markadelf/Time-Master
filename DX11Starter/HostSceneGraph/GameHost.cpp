#include "GameHost.h"
#include "DataNetworkStructs.h"
#include "RequestNetworkStructs.h"
#include "StaticObject.h"
#include "ArenaLevel.h"

GameHost::GameHost(ServerManager* server)
{
    m_ingame = false;
    m_serverPointer = server;
	ColliderManager::get().Reinit(16, 16);
	LoadLevel();
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
		CheckStartGame();
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
    // Recieve
    KeyFrameData key;
    key.Deserialize(data);

	if (m_sceneGraph.GetEntity(playerId)->GetKilledBy().m_entity == -1)
	{

		HostDataHeader header;
		m_sceneGraph.StackKeyFrame(key, header.m_deaths, &header.m_deathCount);
		TemporalEntity* entity = m_sceneGraph.GetEntity(key.m_entityId);

		Phantom phan = entity->Head();

		// Send
		header.m_phantoms[0] = phan;
		header.m_phantomCount = 1;

		if (phan.GetShot()) {
			header.m_phenomina[0] = entity->GetPhenomenaBuffer()[entity->GetPhenomenaCount() - 1];
			header.m_phenominaCount = 1;
		}

		for (size_t i = 0; i < m_sceneGraph.GetEntityCount(); i++)
		{
			Buffer* outData = m_serverPointer->GetNextBufferActiveUser(MessageType::GameData, i);
			header.Serialize(*outData);
			m_serverPointer->SendToActiveUser(i);
		}
		CheckVictory();
	}
}

void GameHost::CheckStartGame()
{
	if (m_clientQueue.GetCount() >= m_sceneGraph.GetEntityCount() && !m_ingame)
	{
		StartGame();
	}
}

void GameHost::StartGame()
{
    m_ingame = true;
	LoadLevel();
	for (int i = 0; i < m_sceneGraph.GetEntityCount(); i++)
    {
        int client;
        m_clientQueue.Pop(client);
        m_serverPointer->SetUser(client, i);
        Buffer* buffer = m_serverPointer->GetNextBufferClient(MessageType::GameRequest, client);

        // Prepare request
        HostRequest request;
        request.m_request = HostRequestType::Prepare;

		request.m_arg = i;

        request.Serialize(*buffer);

        m_serverPointer->SendToClient(client);
    }
}

void GameHost::LoadLevel()
{
    ArenaLevel level;
    level.LoadScene(m_sceneGraph);
}

void GameHost::CheckVictory()
{
	int eCount;
	TemporalEntity* entities;
	m_sceneGraph.GetEntities(&entities, eCount);
	int live = 0;
	int liveIndex = 0;
	for (int i = 0; i < eCount; i++)
	{
		if (!entities[i].GetDead())
		{
			live++;
			liveIndex = i;
		}
	}
	if (live == 1)
	{
		for (size_t i = 0; i < eCount; i++)
		{
			Buffer* buffer = m_serverPointer->GetNextBufferActiveUser(MessageType::GameRequest, i);

			// Prepare request
			HostRequest request;
			request.m_request = HostRequestType::DeclareVictor;

			request.m_arg = liveIndex;

			request.Serialize(*buffer);

			m_serverPointer->SendToActiveUser(i);
		}
		m_ingame = false;
		m_serverPointer->ClearUsers();
		CheckStartGame();
	}
}

