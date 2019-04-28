#pragma once

#include "StaticObject.h"
#include "TemporalEntity.h"
#include "ColliderManager.h"
#include "PlayerKeyFrameData.h"

class SceneGraph
{
	// Player Buffer
	TemporalEntity* m_entities;

	// Static Object Buffer
	StaticObject* m_statics;

    // Phenomena info buffer
    PhenomenaPrototype* m_phenomenaTypes;

	// Number of entities
	int m_entityCount;

	// Number of StaticObjects
	int m_staticObjectCount;

    // Number of varieties of phenomena
    int m_phenomenaTypeCount;

public:
	SceneGraph();
	~SceneGraph();

	void Init(int entityCount);
	void Init(StaticObject* staticObjs, int staticobjectCount);
    void Init(PhenomenaPrototype* phenomenaTypes, int count);

	// Use keyframe data to modify an entity by adding actions to the top of its stack
	void StackKeyFrame(KeyFrameData phantom);
	// Modifies the transform to avoid collisions
	bool PreventCollision(int entityId, Transform& position);

	void GetStatics(StaticObject** objs, int& count);

    void GetEntities(TemporalEntity** ents, int& count);

	TemporalEntity* GetEntity(int index);
	int GetEntityCount() const;
};

