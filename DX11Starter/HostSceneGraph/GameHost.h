#pragma once
#include "ServerManager.h"
#include "ServerSceneGraph.h"
#include "Queue.h"

class GameHost
{
	static const int PLAYERS_PER_SESSION = 2;

	Queue<int, 20> m_clientQueue;
	ServerSceneGraph m_sceneGraph;
	ServerManager* m_serverPointer;
	bool m_inactive;

public:
	GameHost(ServerManager* server);
	~GameHost();

	void HostRecieveClient(Buffer& data, int clientId);
	void HostRecievePlayer(Buffer& data, int playerId);

private:
	void StartGame();

};

