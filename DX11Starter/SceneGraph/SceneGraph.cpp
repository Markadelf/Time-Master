#include "SceneGraph.h"


SceneGraph::SceneGraph()
{
	m_entities = nullptr;
	m_statics = nullptr;
	m_entityCount = 0;
	m_maxEntities = 0;
	m_staticObjectCount = 0;
}

SceneGraph::~SceneGraph()
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
}

void SceneGraph::Init(int maxEntities, int causalityPerEntity)
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

	m_maxEntities = maxEntities;
	m_entityCount = 0;

	m_entities = new TemporalEntity[maxEntities];
	float min[] = { -1000, -1000, -1000 };
	float max[] = { 1000, 1000, 1000 };
}

void SceneGraph::Init(StaticObject* staticObjs, int staticobjectCount)
{
	// Static Object Buffer
	m_statics = new StaticObject[staticobjectCount];
	memcpy(m_statics, staticObjs, staticobjectCount * sizeof(StaticObject));

	// Number of StaticObjects
	m_staticObjectCount = staticobjectCount;
}

void SceneGraph::StackKeyFrame(KeyFrameData keyFrame)
{
	// TODO: Check for collisions
	TemporalEntity* entity = &m_entities[keyFrame.m_entityId];
	HandleObject handle = entity->GetHandle();
	float min[3];
	float max[3];
	Phantom* phantom = entity->StackKeyFrame(keyFrame);
	if (keyFrame.m_shot && phantom != nullptr)
	{
		// TODO: MAKE DIFFERENT BULLET TYPES POSSIBLE
		Transform transform = phantom->GetTransform().GetTransform(keyFrame.m_shotTime);
		const float BULLETRANGE = 10;
		const TimeStamp BULLETPERIOD = 2;
		Vector2 finalPos = transform.GetPos() + Vector2(0, BULLETRANGE).Rotate(-transform.GetRot());
		TimeInstableTransform traj = TimeInstableTransform(transform, Transform(finalPos, transform.GetRot()), keyFrame.m_shotTime, keyFrame.m_shotTime + BULLETPERIOD, false);

		TimeStamp timeStamp;
		TimeStamp firstTimeStamp = traj.GetReversed() ? traj.GetStartTime() : traj.GetEndTime();
		// Check for collisions with walls
		for (size_t i = 0; i < m_staticObjectCount; i++)
		{
			// TODO: DONT USE THE SHOOTER'S COLLIDER
			if (ColliderManager::get().CheckCollision(traj, handle.m_collider, m_statics[i].GetTransform(), m_statics[i].GetHandles().m_collider, timeStamp))
			{
				if (traj.GetReversed() ? firstTimeStamp < timeStamp : firstTimeStamp > timeStamp)
				{
					firstTimeStamp = timeStamp;
				}
			}
		}
		traj.Trim(firstTimeStamp);

		// TODO: Consider refactoring for more generic logic
		// Check for collisions between the projectile and other entities
		for (size_t i = 0; i < m_entityCount; i++)
		{
			// Bullets don't collide with shooter
			if (i == keyFrame.m_entityId)
			{
				continue;
			}
			Phantom* pBuffer = m_entities[i].GetPhantomBuffer();
			int pCount = m_entities[i].GetImageCount();
			// For each entity, check each image
			for (size_t j = 0; j < pCount; j++)
			{
				// TODO: DONT USE THE SHOOTER'S COLLIDER
				if (ColliderManager::get().CheckCollision(traj, handle.m_collider, pBuffer[j].GetTransform(), m_entities[i].GetHandle().m_collider, timeStamp))
				{
					// We hit, let everyone know we died
					PhenominaHandle reset;
					m_entities[i].Kill((int)j, timeStamp, PhenominaHandle(keyFrame.m_entityId, entity->GetPhenominaCount()), reset);
					for (size_t k = 0; k < m_entityCount; k++)
					{
						if (m_entities[k].CheckRevive(reset))
						{
							m_entities[k].Revive();
						}
					}
				}
			}
		}

		// TODO: MAKE DIFFERENT BULLET HANDLES POSSIBLE
		HandleObject bulletHandle = HandleObject();
		bulletHandle.m_material = 1;
		bulletHandle.m_mesh = 3;
		bulletHandle.SetUniformScale(.5f);

		entity->TrackPhenomina(Phenomina(traj, bulletHandle), keyFrame.m_shotTime);
	}
}

bool SceneGraph::PreventCollision(int entityId, Transform& trans) {
	TemporalEntity* entity = &m_entities[entityId];
	HandleObject handle = entity->GetHandle();
	Vector2 overlap;
	Vector2 delta;
	bool ret = false;
	for (size_t i = 0; i < m_staticObjectCount; i++)
	{
		if (ColliderManager::get().CheckCollision(trans, handle.m_collider, m_statics[i].GetTransform(), m_statics[i].GetHandles().m_collider, delta, overlap))
		{
			Vector2 newP = trans.GetPos();
			newP = newP + overlap;
			trans.SetPos(newP);
			ret = true;
		}
	}
	return ret;
}

void SceneGraph::GetStatics(StaticObject** objs, int& count) {
	*objs = m_statics;
	count = m_staticObjectCount;
}

TemporalEntity* SceneGraph::GetEntity(int index)
{
	return &m_entities[index];
}

int SceneGraph::GetEntityCount() const
{
	return m_entityCount;
}

int SceneGraph::AddEntity(int maxImages, int maxPhenomina)
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

void ServerSceneGraph::ClientRecieve(Buffer& data)
{
	KeyFrameData key;
	key.Deserialize(data);

	TemporalEntity* entity = &m_entities[key.m_entityId];

	Phantom phan;
	phan.Deserialize(data);

	entity->TrackPhantom(phan);

	if (key.m_shot) {
		Phenomina bull;
		bull.Deserialize(data);
		entity->TrackPhenomina(bull, key.m_shotTime);
	}
}

void ServerSceneGraph::HostRecieve(Buffer& data, int playerId, ServerManager* server)
{
	KeyFrameData key;
	key.Deserialize(data);

	StackKeyFrame(key);
	TemporalEntity* entity = &m_entities[key.m_entityId];

	Phantom phan = entity->Head();

	Buffer* outData = server->GetNextBufferActiveUser(MessageType::GameData, playerId);
	key.Serialize(*outData);
	phan.Serialize(*outData);

	if (key.m_shot) {
		Phenomina bull = entity->GetPhenominaBuffer()[entity->GetPhenominaCount() - 1];
		bull.Serialize(*outData);
	}

	server->SendToActiveUser(playerId);
}

}
