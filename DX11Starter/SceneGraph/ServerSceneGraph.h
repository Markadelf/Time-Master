#pragma once

#include "StaticObject.h"
#include "PlayerObject.h"
#include "ColliderManager.h"
#include "ProjectileManager.h"
#include "PhantomImageData.h"

class ServerSceneGraph
{
	// Manager
	ColliderManager m_colliderManager;
	ProjectileManager m_projectileManager;

	// Buffers
	
	// Player Buffer
	PlayerObject* m_players;

	// Static Object Buffer
	StaticObject* m_statics;

	// Parameters

	// Number of players
	int m_playerCount;
	// Number of StaticObjects
	int m_staticObjectCount;
	
	// Time limit of match in real time
	float m_matchTimeRealTime;
	// Time constarints of match space
	TimeStamp m_matchTimeRangeGameTime;

	// Variables

	// Timer
	float m_matchTimer;
	
public:
	ServerSceneGraph();
	ServerSceneGraph(int teams, int playersPerTeam, int maxBullets, int maxColliders, StaticObject* staticObjs, int staticobjectCount);
	~ServerSceneGraph();


	void StackPhantom(PhantomImageData phantom);
	// Collision Checks
};

