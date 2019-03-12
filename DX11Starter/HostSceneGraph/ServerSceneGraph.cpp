#include "ServerSceneGraph.h"


ServerSceneGraph::ServerSceneGraph()
{
	m_entities = nullptr;
	m_statics = nullptr;
}

ServerSceneGraph::ServerSceneGraph(int maxEntities, int causalityPerEntity, int maxColliders)
{
	m_maxEntities = maxEntities;
	m_entityCount = 0;
	m_colliderManager = new ColliderManager(maxColliders, maxColliders);

	m_entities = new TemporalEntity[maxEntities];

	m_statics = nullptr;

	// Number of StaticObjects
	m_staticObjectCount = 0;
}

ServerSceneGraph::~ServerSceneGraph()
{
	if (m_entities)
	{
		delete[] m_entities;
		m_entities = nullptr;
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

void ServerSceneGraph::Init(StaticObject * staticObjs, int staticobjectCount)
{
	// Static Object Buffer
	m_statics = new StaticObject[staticobjectCount];
	memcpy(m_statics, staticObjs, staticobjectCount * sizeof(StaticObject));

	// Number of StaticObjects
	m_staticObjectCount = staticobjectCount;
}

void ServerSceneGraph::StackKeyFrame(KeyFrameData keyFrame)
{
	// TODO: Check for collisions
	TemporalEntity* entity = &m_entities[keyFrame.m_entityId];
	HandleObject handle = entity->GetHandle();
	Vector2 delta;
	Vector2 overlap;
	for (size_t i = 0; i < m_staticObjectCount; i++)
	{
		if (m_colliderManager->CheckCollision(keyFrame.m_transform, handle.m_collider, m_statics[i].GetTransform(), m_statics[i].GetHandles().m_collider, delta, overlap))
		{
			Vector2 newP = keyFrame.m_transform.GetPos();
			newP = newP + overlap;
			keyFrame.m_transform.SetPos(newP);
		}
	}

	entity->StackKeyFrame(keyFrame);
}

bool ServerSceneGraph::PreventCollision(int entityId, Transform& trans) {
	TemporalEntity* entity = &m_entities[entityId];
	HandleObject handle = entity->GetHandle();
	Vector2 overlap;
	Vector2 delta;
	bool ret = false;
	for (size_t i = 0; i < m_staticObjectCount; i++)
	{
		if (m_colliderManager->CheckCollision(trans, handle.m_collider, m_statics[i].GetTransform(), m_statics[i].GetHandles().m_collider, delta, overlap))
		{
			Vector2 newP = trans.GetPos();
			newP = newP + overlap;
			trans.SetPos(newP);
			ret = true;
		}
	}
	return ret;
}

void ServerSceneGraph::GetStatics(StaticObject** objs, int& count) {
	*objs = m_statics;
	count = m_staticObjectCount;
}

TemporalEntity* ServerSceneGraph::GetEntity(int index)
{
	return &m_entities[index];
}

int ServerSceneGraph::AddEntity(int maxImages, int maxPhenomina)
{
	m_entities[m_entityCount].Initialize(maxImages, maxPhenomina, m_entityCount);
	return m_entityCount++;
}

Colliders2D::ColliderHandle ServerSceneGraph::GetColliderHandle(Colliders2D::ColliderType cType, float a, float b)
{
	switch (cType)
	{
	case Colliders2D::Circle:
		return m_colliderManager->GetCircleHandle(a);
		break;
	case Colliders2D::Rectangle:
		return m_colliderManager->GetRectangularHandle(a, b);
		break;
	default:
		return Colliders2D::ColliderHandle();
	}
}

