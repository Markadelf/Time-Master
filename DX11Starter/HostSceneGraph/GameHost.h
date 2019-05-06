#pragma once
#include "ServerManager.h"
#include "SceneGraph.h"
#include "Queue.h"

class GameHost
{
	Queue<int, 20> m_clientQueue;
	SceneGraph m_sceneGraph;
	ServerManager* m_serverPointer;
	bool m_ingame;

public:
	GameHost(ServerManager* server);
	~GameHost();

	void HostRecieveClient(Buffer& data, int clientId);
	void HostRecievePlayer(Buffer& data, int playerId);

	void ValidateGameState();

private:
	void CheckStartGame();
	void StartGame();
    void LoadLevel();


	void CheckVictory();
	void EndGame();
};

