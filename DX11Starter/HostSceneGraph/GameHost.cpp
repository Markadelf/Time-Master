#include "GameHost.h"
#include "DataNetworkStructs.h"
#include "RequestNetworkStructs.h"
#include "StaticObject.h"
#include "ArenaLevel.h"

GameHost::GameHost(ServerManager* server)
{
    m_ingame = false;
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
        if (m_clientQueue.GetCount() >= PLAYERS_PER_SESSION && !m_ingame)
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
    // Recieve
    KeyFrameData key;
    key.Deserialize(data);

    m_sceneGraph.StackKeyFrame(key);
    TemporalEntity* entity = m_sceneGraph.GetEntity(key.m_entityId);

    Phantom phan = entity->Head();

    // Send
    HostDataHeader header;
    header.m_phantoms[0] = phan;
    header.m_phantomCount = 1;

    if (phan.GetShot()) {
        header.m_phenomina[0] = entity->GetPhenomenaBuffer()[entity->GetPhenomenaCount() - 1];
        header.m_phenominaCount = 1;
    }

	for (size_t i = 0; i < PLAYERS_PER_SESSION; i++)
	{
		Buffer* outData = m_serverPointer->GetNextBufferActiveUser(MessageType::GameData, i);
		header.Serialize(*outData);
		m_serverPointer->SendToActiveUser(i);
	}
}

void GameHost::StartGame()
{
    m_ingame = true;
    for (int i = 0; i < PLAYERS_PER_SESSION; i++)
    {
        int client;
        m_clientQueue.Pop(client);
        m_serverPointer->SetUser(client, i);
        Buffer* buffer = m_serverPointer->GetNextBufferClient(MessageType::GameRequest, client);

        // Prepare request
        //HostRequest request;
        //request.m_request = HostRequestType::Prepare;
        //request.m_time = ...

        GamePreparationRequest prepare;
        prepare.m_playerEntityId = i;
        prepare.m_scene = 0;

        //request.Serialize(*buffer);
        prepare.Serialize(*buffer);

        m_serverPointer->SendToClient(client);
    }
    LoadLevel();
}

void GameHost::LoadLevel()
{
    ArenaLevel level;
    level.LoadScene(m_sceneGraph);
}
