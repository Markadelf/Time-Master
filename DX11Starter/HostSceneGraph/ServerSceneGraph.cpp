#include "ServerSceneGraph.h"


ServerSceneGraph::ServerSceneGraph()
{
	m_players = nullptr;
	m_statics = nullptr;
}

ServerSceneGraph::ServerSceneGraph(int maxEntities, int causalityPerEntity, int maxColliders, StaticObject* staticObjs, int staticobjectCount) 
{
	m_maxEntities = maxEntities;
	m_entityCount = 0;
	m_colliderManager = new ColliderManager(maxColliders, maxColliders);

	m_players = new TemporalEntity[maxEntities];

	// Static Object Buffer
	m_statics = new StaticObject[staticobjectCount];
	memcpy(m_statics, staticObjs, staticobjectCount * sizeof(StaticObject));

	// Number of StaticObjects
	m_staticObjectCount = staticobjectCount;
}

ServerSceneGraph::~ServerSceneGraph()
{
	if (m_players)
	{
		delete[] m_players;
		m_players = nullptr;
	}
	if (m_statics) {
		delete[] m_statics;
		m_statics = nullptr;
	}
	if (m_colliderManager)
	{
		delete m_colliderManager;
		m_colliderManager = nullptr;
	}
}

void ServerSceneGraph::StackKeyFrame(PlayerKeyFrameData keyFrame)
{
	int playerId = keyFrame.GetPlayerId();
	// TODO: Check for collisions
	m_players[playerId].StackKeyFrame(keyFrame);

}

void ServerSceneGraph::GetStatics(StaticObject** objs, int& count) {
	*objs = m_statics;
	count = m_staticObjectCount;
}

TemporalEntity* ServerSceneGraph::GetPlayerPhantoms(int index)
{
	return &m_players[index];
}

int ServerSceneGraph::AddEntity(TemporalEntity & entity)
{
	return 0;
}

