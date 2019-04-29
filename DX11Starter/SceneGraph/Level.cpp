#include "Level.h"

Level::Level()
{
    m_staticObjs = nullptr;
    m_staticObjectCount = 0;

    m_entities = nullptr;
    m_entityCount = 0;

    m_phenomenaTypes = nullptr;
    m_pTypeCount = 0;

#ifdef CLIENT
    m_lightList = nullptr;
    m_lightCount = 0;
#endif // CLIENT
}

Level::~Level()
{
    if (m_staticObjs)
    {
        delete[] m_staticObjs;
        m_staticObjs = nullptr;
    }
    if (m_entities)
    {
        delete[] m_entities;
        m_entities = nullptr;
    }
    if (m_phenomenaTypes)
    {
        delete[] m_phenomenaTypes;
        m_phenomenaTypes = nullptr;
    }
#ifdef CLIENT
    if (m_lightList)
    {
        delete[] m_lightList;
        m_lightList = nullptr;
    }
#endif // CLIENT
}

void Level::LoadScene(SceneGraph& graph)
{
    // Initialize static objects
    graph.Init(m_staticObjs, m_staticObjectCount);

    // Initialize Entities
    graph.Init(m_entityCount);

    TemporalEntity* entities;
    int eCount;

    graph.GetEntities(&entities, eCount);

    for (int i = 0; i < eCount; i++)
    {
        EntitySpawnInfo& info = m_entities[i];
        entities[i].Initialize(info.m_maxImages, info.m_maxPhenomena, i);
        entities[i].Initialize(info.m_startingPos, info.m_initialTime, info.m_handle);
        entities[i].SetAction(info.m_action);
    }

    // Initialize phenomena types
    graph.Init(m_phenomenaTypes, m_pTypeCount);
}

EntitySpawnInfo& Level::GetSpawnInfo(int id)
{
    return m_entities[id];
}

#ifdef CLIENT
void Level::GetLights(Light** lights, int& count) {
    *lights = m_lightList;
    count = m_lightCount;
}
#endif // CLIENT
