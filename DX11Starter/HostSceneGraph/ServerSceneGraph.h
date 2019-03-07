#pragma once

#include "StaticObject.h"
#include "TemporalEntity.h"
#include "ColliderManager.h"
#include "PlayerKeyFrameData.h"

class ServerSceneGraph
{
	// Manager
	ColliderManager* m_colliderManager;

	// Buffers
	
	// Player Buffer
	TemporalEntity* m_players;

	// Static Object Buffer
	StaticObject* m_statics;

	// Parameters

	// Number of entities
	int m_maxEntities;
	int m_entityCount;

	// Number of StaticObjects
	int m_staticObjectCount;
	
	// Variables

	// Timer
	float m_matchTimer;
	
public:
	ServerSceneGraph();
	ServerSceneGraph(int maxEntities, int causalityPerEntity, int maxColliders, StaticObject* staticObjs, int staticobjectCount);
	~ServerSceneGraph();

	void StackKeyFrame(PlayerKeyFrameData phantom);

	void GetStatics(StaticObject** objs, int& count);

	TemporalEntity* GetPlayerPhantoms(int index);

	int AddEntity(TemporalEntity& entity);
};

