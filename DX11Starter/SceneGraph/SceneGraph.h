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

	// Tracks if we have broken
	bool m_valid;

public:
	SceneGraph();
	~SceneGraph();

    // Initializes the entity buffer
	void Init(int entityCount);

    // Initializes the static object buffer
	void Init(StaticObject* staticObjs, int staticobjectCount);

    // Initializes the phenomena prototype buffer
    void Init(PhenomenaPrototype* phenomenaTypes, int count);


	// Use keyframe data to modify an entity by adding actions to the top of its stack
	void StackKeyFrame(KeyFrameData phantom);

	// Modifies the transform to avoid collisions with static objects
	bool PreventCollision(int entityId, Transform& position);


    // Accessor functions
	void GetStatics(StaticObject** objs, int& count);
    void GetEntities(TemporalEntity** ents, int& count);
	bool CheckValid();

    TemporalEntity* GetEntity(int index);
	
    int GetEntityCount() const;
};

