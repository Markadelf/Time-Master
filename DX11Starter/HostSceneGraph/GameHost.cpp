#include "GameHost.h"
#include "DataNetworkStructs.h"
#include "RequestNetworkStructs.h"
#include "StaticObject.h"

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
        header.m_phenomina[0] = entity->GetPhenominaBuffer()[entity->GetPhenominaCount() - 1];
        header.m_phenominaCount = 1;
    }

    Buffer* outData = m_serverPointer->GetNextBufferActiveUser(MessageType::GameData, playerId);
    header.Serialize(*outData);
    m_serverPointer->SendToActiveUser(playerId);
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
    LoadLevel();
}

void GameHost::LoadLevel()
{
    // Add static objects to scene graph
    const int div = 20;
    StaticObject objs[div + 1];
    Vector2 right = Vector2(5, 0);
    HandleObject handle;
    handle.m_scale[2] = 2;
    handle.m_collider = ColliderManager::get().GetRectangularHandle(1, 2);

    Transform trans;
    for (size_t i = 0; i < div; i++)
    {
        trans = Transform(right.Rotate(6.28f / div * i), -6.28f / div * i);
        objs[i] = StaticObject(trans, handle);
    }
    handle.SetUniformScale(1);
    handle.m_collider = ColliderManager::get().GetCircleHandle(.5f);
    //handle.m_scale[2] = 1;

    trans = Transform(Vector2(), 0);
    objs[div] = StaticObject(trans, handle);

    m_sceneGraph.Init(16, 100);
    m_sceneGraph.Init(&objs[0], div + 1);

    handle.m_collider = ColliderManager::get().GetCircleHandle(.25f);
    handle.SetUniformScale(1);
    Vector2 pos(0, -3);

    // Add player
    int id = m_sceneGraph.AddEntity(2048, 100);
}
