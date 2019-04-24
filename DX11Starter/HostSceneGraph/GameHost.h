#pragma once
#include "ServerManager.h"
#include "SceneGraph.h"
#include "Queue.h"

class GameHost
{
	static const int PLAYERS_PER_SESSION = 2;

	Queue<int, 20> m_clientQueue;
	SceneGraph m_sceneGraph;
	ServerManager* m_serverPointer;
	bool m_inactive;

public:
	GameHost(ServerManager* server);
	~GameHost();

	void HostRecieveClient(Buffer& data, int clientId);
	void HostRecievePlayer(Buffer& data, int playerId);

//private:
	void StartGame();
    void LoadLevel();

};

