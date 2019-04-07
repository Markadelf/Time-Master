#pragma once
#include "ServerManager.h"
#include "ServerSceneGraph.h"

class GameHost
{
	//stub

	ServerSceneGraph m_sceneGraph;

public:
	GameHost();
	~GameHost();

	void HostRecieveClient(Buffer& data, int playerId, ServerManager* server);
	void HostRecievePlayer(Buffer& data, int playerId, ServerManager* server);
};

