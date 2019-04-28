#pragma once
#include "StaticObject.h"
#include "SceneGraph.h"

#ifdef CLIENT
#include "DXCore.h"
#include "Lights.h"
#endif // CLIENT


// Instructions on how to build an entity
struct EntitySpawnInfo 
{
    Transform m_startingPos;
    HandleObject m_handle;
    ActionInfo m_action;

    int m_maxImages;
    int m_maxPhenomena;
   
    float m_initialTime;
};

// A level is a module that knows which assets to load and such for the scene graph to run a scene
// TODO: Add asset module that interfaces with the asset manager
class Level
{
protected:
    StaticObject* m_staticObjs;
    int m_staticObjectCount;

    EntitySpawnInfo* m_entities;
    int m_entityCount;

    PhenomenaPrototype* m_phenomenaTypes;
    int m_pTypeCount;
    
#ifdef CLIENT
    Light* m_lightList;
    int m_lightCount;
#endif // CLIENT

public:
    Level();
    virtual ~Level();

    // Initializes the level, does not load assets
    void LoadScene(SceneGraph& graph);

    // Fetch the spawn info for a particular Entity
    EntitySpawnInfo& GetSpawnInfo(int id);

#ifdef CLIENT
    // Fetch the lights associated with the level
    void GetLights(Light** lights, int& count);
#endif // CLIENT

};

