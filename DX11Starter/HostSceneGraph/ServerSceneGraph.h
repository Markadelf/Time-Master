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
	TemporalEntity* m_entities;

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
	ServerSceneGraph(int maxEntities, int causalityPerEntity, int maxColliders);
	~ServerSceneGraph();

	void Init(StaticObject* staticObjs, int staticobjectCount);

	void StackKeyFrame(KeyFrameData phantom);
	bool PreventCollision(int entityId, Transform& position);

	void GetStatics(StaticObject** objs, int& count);

	TemporalEntity* GetEntity(int index);
	int GetEntityCount() const;

	int AddEntity(int maxImages, int maxPhenomina);

	Colliders2D::ColliderHandle GetColliderHandle(Colliders2D::ColliderType cType, float a = 0, float b = 0);

};

