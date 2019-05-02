#include "ClientManager.h"
#include "ColliderManager.h"
#include "AssetManager.h"
#include "MathUtil.h"

#include <algorithm>
#include "ArenaLevel.h"

ClientManager::ClientManager()
{
	// This is where we currently initialize the collider manager
	// Eventually we will want to do this when loading the scene
	ColliderManager::get().Reinit(16, 16);
}


ClientManager::~ClientManager()
{
	if (m_drawInfo.emitter != nullptr)
	{
		delete m_drawInfo.emitter;
		m_drawInfo.emitter = nullptr;
	}
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
		m_graph.StackKeyFrame(m_player.GetKeyFrame());
	}
	//m_drawInfo.emitter->Update(deltaTime, totalTime);
}

void ClientManager::Init(ID3D11Device* device)
{
	//TODO: Modify to load either from file or from a preset instead of hard coding it
    ArenaLevel arena;
    arena.LoadScene(m_graph);

    int playerId = 0;
    EntitySpawnInfo& player = arena.GetSpawnInfo(playerId);
    m_player.Initialize(player.m_startingPos, player.m_initialTime, player.m_handle, .1f);
    m_player.SetEntityId(playerId);
    m_player.SetAction(player.m_action);

    Light* lights;
    int lCount;
    arena.GetLights(&lights, lCount);

    memcpy(m_drawInfo.m_lightList, lights, lCount * sizeof(Light));
    m_drawInfo.m_lightCount = lCount;

	if (m_drawInfo.emitter != nullptr)
	{
		delete m_drawInfo.emitter;
		m_drawInfo.emitter = nullptr;
	}
	// Set up particles
	m_drawInfo.emitter = new Emitter(
		10,							// Max particles
		20,								// Particles per second
		.5f,							// Particle lifetime
		0.1f,							// Start size
		2.0f,							// End size
		XMFLOAT4(1, 0.1f, 0.1f, 0.7f),	// Start color
		XMFLOAT4(1, 0.6f, 0.1f, 0),		// End color
		XMFLOAT3(0, 0, 0),				// Start velocity
		XMFLOAT3(1, 1, 1),		        // Velocity randomness range
		XMFLOAT3(0, 0, 0),		        // Emitter position
		XMFLOAT3(0.1f, 0.1f, 0.1f),		// Position randomness range
		XMFLOAT4(-2, 2, -2, 2),			// Random rotation ranges (startMin, startMax, endMin, endMax)
		XMFLOAT3(0, -1, 0),				// Constant acceleration
		device,
		AssetManager::get().GetTextureHandle("Textures/particle.jpg"));

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
    m_drawInfo.m_emitterCount = 0;
    m_drawInfo.m_transparentCount = 0;
	TimeStamp time = m_player.GetTimeStamp();

	m_drawInfo.time = time;



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
			if (trans.GetEndTime() > time && trans.GetStartTime() < time)
			{
				//ItemFromTransHandle(m_drawInfo.m_opaqueObjects[m_drawInfo.m_visibleCount++], trans.GetTransform(time), phenomenas[j].GetHandle());
                // Projectiles
                EmitterDrawInfo& drawInfo = m_drawInfo.m_emitters[m_drawInfo.m_emitterCount++];
                drawInfo.m_handle = 0;
                drawInfo.startTime = trans.GetStartTime();
                drawInfo.endTime = trans.GetEndTime();

                Vector2 trans2 = trans.GetPos(time);
                drawInfo.pos = DirectX::XMFLOAT3(trans2.GetX(), handle.m_yPos, trans2.GetY());
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
