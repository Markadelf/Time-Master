#include "ClientManager.h"
#include "ColliderManager.h"
#include "AssetManager.h"

ClientManager::ClientManager()
{
	// This is where we currently initialize the collider manager
	// Eventually we will want to do this when loading the scene
	ColliderManager::get().Reinit(16, 16);
}


ClientManager::~ClientManager()
{
}

void ClientManager::Update(float deltaTime)
{
	// Get the static object count from the scenegraph again
	// Later, the SCount for the draw group may differ from this one
	int sCount;
	StaticObject* statics;
	m_graph.GetStatics(&statics, sCount);

	// Update the first person controller
	// TODO: Consider permiting more than one fps controller (AI controllers?)
	m_player.Update(deltaTime, statics, sCount);
	Transform trans = m_player.GetTransform();
	m_graph.PreventCollision(m_player.GetEntityId(), trans);

	static int frame = 0;
	if (frame > 30)
	{
		m_graph.StackKeyFrame(m_player.GetKeyFrame());
		frame = 0;
	}
	else
	{
		frame++;
	}
}

void ClientManager::Init()
{
	//TODO: Modify to load either from file or from a preset instead of hard coding it

	int floorMaterial = AssetManager::get().GetMaterialHandle("FLOOR");
	int woodMaterial  = AssetManager::get().GetMaterialHandle("WOOD");
	int playerMaterial = AssetManager::get().GetMaterialHandle("PLAYER3");

	// Load in the files and get the handles for each from the meshManager
	int coneHandle = AssetManager::get().GetMeshHandle("OBJ_Files/cone.obj");
	int cubeHandle = AssetManager::get().GetMeshHandle("OBJ_Files/cube.obj");
	int cylinderHandle = AssetManager::get().GetMeshHandle("OBJ_Files/cylinder.obj");
	int sphereHandle = AssetManager::get().GetMeshHandle("OBJ_Files/sphere.obj");
	int duckHandle = AssetManager::get().GetMeshHandle("OBJ_Files/duck.fbx");

	// Add static objects to scene graph
	const int div = 20;
	StaticObject objs[div + 1];
	Vector2 right = Vector2(5, 0);
	HandleObject handle;
	handle.m_material = woodMaterial;
	handle.m_mesh = cubeHandle;
	handle.m_scale[2] = 2;
	handle.m_collider = ColliderManager::get().GetRectangularHandle(1, 2);

	for (size_t i = 0; i < div; i++)
	{
		objs[i] = (StaticObject(Transform(right.Rotate(6.28f / div * i), -6.28f / div * i), handle));
	}
	handle.m_material = floorMaterial;
	handle.m_mesh = cylinderHandle;
	handle.SetUniformScale(1);
	handle.m_collider = ColliderManager::get().GetCircleHandle(.5f);
	//handle.m_scale[2] = 1;

	objs[div] = (StaticObject(Transform(Vector2(), 0), handle));

	m_graph.Init(16, 100);
	m_graph.Init(&objs[0], div + 1);

	handle.m_material = playerMaterial;
	handle.m_mesh = cubeHandle;
	handle.m_collider = ColliderManager::get().GetCircleHandle(.25f);
	handle.SetUniformScale(1);
	Vector2 pos(0, -3);

	// Add player
	int id = m_graph.AddEntity(2048, 100);
	m_player.Initialize(Transform(pos, 0), 0, handle);
	m_graph.GetEntity(id)->Initialize(Transform(pos, 0), m_player.GetTimeStamp(), handle);
	m_player.SetEntityId(id);

	PrepDrawGroupStatics();

	//Initiating lighting
	Light directLight, spotLight, pointLight;

	directLight.Type = LIGHT_TYPE_DIRECTIONAL;
	directLight.Direction = DirectX::XMFLOAT3(-1, 0, 0);
	directLight.Color = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f);
	directLight.DiffuseIntensity = 1.0f;
	directLight.AmbientIntensity = 0.4f;

	pointLight.Type = LIGHT_TYPE_POINT;
	pointLight.Position = DirectX::XMFLOAT3(-3, -3, 0);
	pointLight.Direction = DirectX::XMFLOAT3(1, 1, 0);
	pointLight.Range = 20.0f;
	pointLight.Color = DirectX::XMFLOAT3(1.0f, 0.1f, 0);
	pointLight.DiffuseIntensity = 1.0f;
	pointLight.AmbientIntensity = 0.0f;

	spotLight.Type = LIGHT_TYPE_SPOT;
	spotLight.Position = DirectX::XMFLOAT3(0, 5, 0);
	spotLight.Direction = DirectX::XMFLOAT3(0, -1, 0);

	spotLight.Range = 20.0f;
	spotLight.Color = DirectX::XMFLOAT3(1, 0, 1);
	spotLight.SpotFalloff = 25.0f;
	spotLight.DiffuseIntensity = 1.0f;
	spotLight.AmbientIntensity = 0.1f;

	m_drawInfo.m_lightList[0] = directLight;
	m_drawInfo.m_lightList[1] = pointLight;
	m_drawInfo.m_lightList[2] = spotLight;

	m_drawInfo.m_lightCount = 3;
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
	TimeStamp time = m_player.GetTimeStamp();

	int eCount = m_graph.GetEntityCount();
	for (size_t i = 0; i < eCount; i++)
	{
		TemporalEntity* entity = m_graph.GetEntity(i);
		HandleObject handle = entity->GetHandle();

		int phanCount = entity->GetImageCount();
		Phantom* phantoms = entity->GetPhantomBuffer();
		
		for (size_t j = 0; j < phanCount; j++)
		{
			TimeInstableTransform trans = phantoms[j].GetTransform();
			if (trans.GetEndTime() > time && trans.GetStartTime() < time)
			{
				ItemFromTransHandle(m_drawInfo.m_opaqueObjects[m_drawInfo.m_visibleCount++], trans.GetTransform(time), handle);
			}
		}

		int phenCount = entity->GetPhenominaCount();
		Phenomina* phenomenas = entity->GetPhenominaBuffer();

		for (size_t j = 0; j < phenCount; j++)
		{
			TimeInstableTransform trans = phenomenas[j].GetTransform();
			if (trans.GetEndTime() > time && trans.GetStartTime() < time)
			{
				ItemFromTransHandle(m_drawInfo.m_opaqueObjects[m_drawInfo.m_visibleCount++], trans.GetTransform(time), phenomenas[j].GetHandle());
			}
		}
	}
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
