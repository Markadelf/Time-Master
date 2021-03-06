#include "SceneGraph.h"


SceneGraph::SceneGraph()
{
	m_entities = nullptr;
	m_statics = nullptr;
	m_phenomenaTypes = nullptr;
	m_entityCount = 0;
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
	if (m_phenomenaTypes) {
		delete[] m_phenomenaTypes;
		m_phenomenaTypes = nullptr;
	}
}

void SceneGraph::Init(int entityCount)
{
	if (m_entities)
	{
		delete[] m_entities;
		m_entities = nullptr;
	}

	m_entityCount = entityCount;

	m_entities = new TemporalEntity[entityCount];
	m_valid = true;
}

void SceneGraph::Init(StaticObject* staticObjs, int staticobjectCount)
{
	if (m_statics) {
		delete[] m_statics;
		m_statics = nullptr;
	}

	// Static Object Buffer
	m_statics = new StaticObject[staticobjectCount];
	memcpy(m_statics, staticObjs, staticobjectCount * sizeof(StaticObject));

	// Number of StaticObjects
	m_staticObjectCount = staticobjectCount;
}

void SceneGraph::Init(PhenomenaPrototype* phenomenaTypes, int count)
{
	if (m_phenomenaTypes) {
		delete[] m_phenomenaTypes;
		m_phenomenaTypes = nullptr;
	}

	// Phenomena prototype Buffer
	m_phenomenaTypes = new PhenomenaPrototype[count];
	memcpy(m_phenomenaTypes, phenomenaTypes, count * sizeof(PhenomenaPrototype));

	// Number of prototypes
	m_phenomenaTypeCount = count;
}

void SceneGraph::StackKeyFrame(KeyFrameData keyFrame, DeathInfo* deathBuffer, int* deathCount)
{
	TemporalEntity* entity = &m_entities[keyFrame.m_entityId];
	if (entity->GetKilledBy().m_entity != -1) {
		return;
	}
	HandleObject handle = entity->GetHandle();
	Phantom* phantom = entity->StackKeyFrame(keyFrame);
	if (phantom == nullptr)
	{
		m_valid = false;
		return;
	}
	int dCount = 0;
	if (keyFrame.m_usedAction)
	{
		ActionInfo action = entity->GetAction();
		float shotTime = keyFrame.m_timeStamp + action.m_deploymentTime;
		PhenomenaPrototype& proto = m_phenomenaTypes[action.m_phenomenaType];

		// Stack a keyframe for this action
		Transform transform = phantom->GetTransform().GetTransform(shotTime);

		Vector2 finalPos = transform.GetPos() + Vector2(0, proto.m_range).Rotate(-transform.GetRot());
		TimeInstableTransform traj = TimeInstableTransform(transform, Transform(finalPos, transform.GetRot()), shotTime, shotTime + proto.m_period, false);

		TimeStamp timeStamp;
		TimeStamp firstTimeStamp = traj.GetReversed() ? traj.GetStartTime() : traj.GetEndTime();
		// Check for collisions with walls
		for (size_t i = 0; i < m_staticObjectCount; i++)
		{
			if (ColliderManager::get().CheckCollision(traj, proto.m_handle.m_collider, m_statics[i].GetTransform(), m_statics[i].GetHandles().m_collider, timeStamp))
			{
				// We only care about the earliest collision
				if (traj.GetReversed() ? firstTimeStamp < timeStamp : firstTimeStamp > timeStamp)
				{
					firstTimeStamp = timeStamp;
				}
			}
		}
		traj.Trim(firstTimeStamp);

		// TODO: Consider refactoring for more generic logic
		// Check for collisions between the projectile and other entities
		for (int i = 0; i < m_entityCount; i++)
		{
			// Bullets don't collide with shooter
			if (i == keyFrame.m_entityId)
			{
				continue;
			}
			Phantom* pBuffer = m_entities[i].GetPhantomBuffer();
			int pCount = m_entities[i].GetImageCount();
			// For each entity, check each image
			for (int j = 0; j < pCount; j++)
			{
				if (ColliderManager::get().CheckCollision(traj, proto.m_handle.m_collider, pBuffer[j].GetTransform(), m_entities[i].GetHandle().m_collider, timeStamp))
				{
					PhenomenaHandle dHandle = PhenomenaHandle(keyFrame.m_entityId, entity->GetPhenomenaCount());
					if (deathBuffer != nullptr) {
						deathBuffer[dCount].m_entityId = i;
						deathBuffer[dCount].m_image = j;
						deathBuffer[dCount].m_deathTime = timeStamp;
						deathBuffer[dCount].m_killedBy = dHandle;
						dCount++;
					}

					// We hit, let everyone know we died
					Kill(m_entities[i], j, timeStamp, dHandle);
				}
			}
		}


		if (!entity->TrackPhenomena(Phenomenon(traj, proto.m_handle))) {
			m_valid = false;
		}
	}
	TimeInstableTransform trans = phantom->GetTransform();

	// Check if we ran into any bullets
	for (int i = 0; i < m_entityCount; i++)
	{
		// Bullets don't collide with shooter
		if (i == keyFrame.m_entityId)
		{
			continue;
		}
		Phenomenon* pBuffer = m_entities[i].GetPhenomenaBuffer();
		int pCount = m_entities[i].GetPhenomenaCount();
		// For each entity, check each projectile
		for (int j = 0; j < pCount; j++)
		{
			TimeStamp timeStamp;
			TimeStamp firstTimeStamp = trans.GetReversed() ? trans.GetStartTime() : trans.GetEndTime();
			if (ColliderManager::get().CheckCollision(pBuffer[j].GetTransform(), pBuffer[j].GetHandle().m_collider, trans, entity->GetHandle().m_collider, timeStamp))
			{
				PhenomenaHandle dHandle = PhenomenaHandle(i, j);
				if (deathBuffer != nullptr) {
					deathBuffer[dCount].m_entityId = keyFrame.m_entityId;
					deathBuffer[dCount].m_image = entity->GetImageCount() - 1;
					deathBuffer[dCount].m_deathTime = timeStamp;
					deathBuffer[dCount].m_killedBy = dHandle;
					dCount++;
				}
				// We hit, let everyone know we died
				Kill(*entity, entity->GetImageCount() - 1, timeStamp, dHandle);
			}
		}
	}
	// If someone is tracking deaths, give them the info
	if (deathCount != nullptr)
	{
		*deathCount = dCount;
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

void SceneGraph::GetEntities(TemporalEntity** ents, int& count)
{
	*ents = m_entities;
	count = m_entityCount;
}

bool SceneGraph::CheckValid()
{
	return m_valid;
}

TemporalEntity* SceneGraph::GetEntity(int index)
{
	return &m_entities[index];
}

int SceneGraph::GetEntityCount() const
{
	return m_entityCount;
}

void SceneGraph::Kill(TemporalEntity& entity, int image, float timeStamp, PhenomenaHandle dHandle)
{
	PhenomenaHandle reset;
	entity.Kill(image, timeStamp, dHandle, reset);
	for (size_t k = 0; k < m_entityCount; k++)
	{
		if (m_entities[k].CheckRevive(reset))
		{
			m_entities[k].Revive();
		}
	}
}

int SceneGraph::AddEntity(int maxImages, int maxPhenomina)
{
	m_entities[m_entityCount].Initialize(maxImages, maxPhenomina, m_entityCount);
	return m_entityCount++;
}

void SceneGraph::AuthoritativeStack(HostDataHeader& authoritativeHeader)
{
	int phenominaIndex = 0;
	// Add any phantoms the server told us to
	for (size_t i = 0; i < authoritativeHeader.m_phantomCount; i++)
	{
		Phantom& phantom = authoritativeHeader.m_phantoms[i];
		TemporalEntity& entity = m_entities[phantom.GetEntityId()];

		// If there is a phenomina for this keyframe, track it
		if (!entity.TrackPhantom(phantom)) {
			m_valid = false;
		}
		if (phantom.GetShot())
		{
			if (!entity.TrackPhenomena(authoritativeHeader.m_phenomina[phenominaIndex])) {
				m_valid = false;
			}
			phenominaIndex++;
		}
	}
	// Kill anyone the server said to kill
	for (size_t i = 0; i < authoritativeHeader.m_deathCount; i++)
	{
		DeathInfo death = authoritativeHeader.m_deaths[i];
		Kill(m_entities[death.m_entityId], death.m_image, death.m_deathTime, death.m_killedBy);
	}
}

