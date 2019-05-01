#include "ClientManager.h"
#include "ColliderManager.h"
#include "AssetManager.h"
#include "DataNetworkStructs.h"
#include "MathUtil.h"

#include <algorithm>
#include "ArenaLevel.h"

ClientManager::ClientManager()
{
	// This is where we currently initialize the collider manager
	// Eventually we will want to do this when loading the scene
	ColliderManager::get().Reinit(16, 16);
    m_networkConnection = nullptr;
}


ClientManager::~ClientManager()
{
}

void ClientManager::Update(float deltaTime)
{
	// Update the first person controller
	// TODO: Consider permiting more than one fps controller (AI controllers?)
	m_player.Update(deltaTime);
	Transform trans = m_player.GetTransform();
	m_graph.PreventCollision(m_player.GetEntityId(), trans);
	m_player.SetTransform(trans);

	if (m_player.StackRequested())
	{
        if (m_networkConnection == nullptr)
        {
            // Offline logic
			m_graph.StackKeyFrame(m_player.GetKeyFrame());
        }
        else 
        {
            // Online logic
            Buffer* buffer = m_networkConnection->GetNextBuffer(MessageType::GameData);
            m_player.GetKeyFrame().Serialize(*buffer);
            m_networkConnection->SendToServer();
        }
	}
}

void ClientManager::Init(int entityId)
{
	//TODO: Modify to load either from file or from a preset instead of hard coding it
    ArenaLevel arena;
    arena.LoadScene(m_graph);

    EntitySpawnInfo& player = arena.GetSpawnInfo(entityId);
    m_player.Initialize(player.m_startingPos, player.m_initialTime, player.m_handle, .1f);
    m_player.SetEntityId(entityId);
    m_player.SetAction(player.m_action);

    Light* lights;
    int lCount;
    arena.GetLights(&lights, lCount);

    memcpy(m_drawInfo.m_lightList, lights, lCount * sizeof(Light));
    m_drawInfo.m_lightCount = lCount;

    PrepDrawGroupStatics();
}

Player& ClientManager::GetPlayer()
{
	return m_player;
}

SceneGraph& ClientManager::GetSceneGraph()
{
	return m_graph;
}

DrawGroup& ClientManager::GetDrawGroup()
{
	PrepDrawGroup();
	return m_drawInfo;
}

void ClientManager::SetNetworkPointer(ClientHelper* connection)
{
    m_networkConnection = connection;
}

void ClientManager::RecieveData(Buffer& data)
{
    HostDataHeader header;
    
    header.Deserialize(data);
    m_graph.AuthoritativeStack(header);
}

void ClientManager::PrepDrawGroupStatics()
{
	StaticObject* objs;
	m_graph.GetStatics(&objs, m_staticCount);
	for (size_t i = 0; i < m_staticCount; i++)
	{
		ItemFromTransHandle(m_drawInfo.m_opaqueObjects[i], objs[i].GetTransform(), objs[i].GetHandles());
	}

	m_drawInfo.m_visibleCount = m_staticCount;
}

void ClientManager::PrepDrawGroup()
{
	// Camera
	DirectX::XMFLOAT3 pos;
	Transform player = m_player.GetTransform();
	pos.x = player.GetPos().GetX();
	pos.y = 0;
	pos.z = player.GetPos().GetY();
	m_drawInfo.m_camera.SetPosition(pos);
	m_drawInfo.m_camera.SetYaw(player.GetRot());

	// Entities
    m_drawInfo.m_visibleCount = m_staticCount;
    m_drawInfo.m_transparentCount = 0;
	TimeStamp time = m_player.GetTimeStamp();

	int eCount = m_graph.GetEntityCount();
	for (size_t i = 0; i < eCount; i++)
	{
		TemporalEntity* entity = m_graph.GetEntity((int)i);
		HandleObject handle = entity->GetHandle();

		int phanCount = entity->GetImageCount();
		Phantom* phantoms = entity->GetPhantomBuffer();
		
        float* pTimeReversed;
        int rCount;

        entity->GetReverseBuffer(&pTimeReversed, rCount);

        // Add phantoms
        const float fadePeriod = .5f;
        int rIndex = 0;
		for (size_t j = 0; j < phanCount; j++)
		{
			TimeInstableTransform trans = phantoms[j].GetTransform();
            if (trans.GetEndTime() > time && trans.GetStartTime() < time)
			{
                float personalTime = phantoms[j].GetPersonalTime() + (trans.GetReversed() ? (trans.GetEndTime() - time) : (time - trans.GetStartTime()));
                float opacity = 1;
                while (personalTime >= pTimeReversed[rIndex + 1])
                {
                    rIndex++;
                }
                if (personalTime <= pTimeReversed[rIndex] + fadePeriod)
                {
                    opacity = (personalTime - pTimeReversed[rIndex]) / fadePeriod;
                }
                if (personalTime + fadePeriod >= pTimeReversed[rIndex + 1])
                {
                    if (opacity != 1)
                    {
                        float opacity2 = (pTimeReversed[rIndex + 1] - personalTime) / fadePeriod;
                        opacity = opacity < opacity2 ? opacity : opacity2;
                    }
                    else
                    {
                        opacity = (pTimeReversed[rIndex + 1] - personalTime) / fadePeriod;
                    }
                }
                opacity = opacity * opacity;
				
				if(opacity == 1)
				{
					ItemFromTransHandle(m_drawInfo.m_opaqueObjects[m_drawInfo.m_visibleCount++], trans.GetTransform(time), handle);
				}
                else if(opacity > 0)
                {
                    TransparentEntity& tEnt = m_drawInfo.m_transparentObjects[m_drawInfo.m_transparentCount++];
					
                    ItemFromTransHandle(tEnt.m_entity, trans.GetTransform(time), handle);
                    tEnt.m_transparency = opacity;
					tEnt.m_distance = mathutil::Distance(m_drawInfo.m_camera.GetPosition(), tEnt.m_entity.GetPosition());
                }
			}
        }

		int phenCount = entity->GetPhenomenaCount();
		Phenomenon* phenomenas = entity->GetPhenomenaBuffer();

		for (size_t j = 0; j < phenCount; j++)
		{
			TimeInstableTransform trans = phenomenas[j].GetTransform();
			if (trans.GetEndTime() > time && trans.GetStartTime() <= time)
			{
				ItemFromTransHandle(m_drawInfo.m_opaqueObjects[m_drawInfo.m_visibleCount++], trans.GetTransform(time), phenomenas[j].GetHandle());
			}
		}
	}

	// sorting transperant entities
	std::sort(m_drawInfo.m_transparentObjects, m_drawInfo.m_transparentObjects + m_drawInfo.m_transparentCount,
			//lambda to define the sorting comparision
			[](TransparentEntity const & first, TransparentEntity const & second)->bool
			{
				return first.m_distance < second.m_distance;
			}	
	);
}

void ClientManager::ItemFromTransHandle(DrawItem& item, Transform trans, HandleObject handle)
{
	DirectX::XMFLOAT3 pos(trans.GetPos().GetX(), handle.m_yPos, trans.GetPos().GetY());
	DirectX::XMFLOAT3 scale(handle.m_scale[0], handle.m_scale[1], handle.m_scale[2]);
	DirectX::XMFLOAT3 rot(0, trans.GetRot(), 0);
	
	item.SetPosition(pos);
	item.SetScale(scale);
	item.SetRotation(rot);

	item.SetMeshHandle(handle.m_mesh);
	item.SetMaterialHandle(handle.m_material);
}
