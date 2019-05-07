#include "ArenaLevel.h"
#include "StaticObject.h"

ArenaLevel::ArenaLevel()
{
	int wallMaterial = 3;
	int floorMaterial = 2;
	int woodMaterial = 1;
	int playerMaterial = 0;

	int coneHandle = 0;
	int cubeHandle = 1;
	int cylinderHandle = 2;
	int sphereHandle = 3;
	int duckHandle = 4;
	int planeHandle = 5;


	// Add static objects to scene graph
	HandleObject handle;
	const int floor = 64;
	const int wall = 32;
	const int sept = 10;
	StaticObject objs[floor + wall + sept];
	Transform trans;

	//Floor	
	handle.m_material = floorMaterial;
	handle.m_mesh = cubeHandle;
	handle.SetUniformScale(1);
	handle.m_scale[1] = 0.1f;
	handle.m_yPos = -0.5f;
	int ctr = 0;
	for (float i = 0; i < 8; i++)
	{
		for (float j = 0; j < 8; j++)
		{
			trans = Transform(Vector2(0 + i, 0 + j), 0);
			objs[ctr] = StaticObject(trans, handle);
			ctr++;
		}
	}

	//Boundary Wall	
	handle.m_material = woodMaterial;
	handle.m_mesh = cubeHandle;
	handle.SetUniformScale(1);
	handle.m_scale[2] = 0.1f;
	handle.m_yPos = 0;
	handle.m_collider = ColliderManager::get().GetRectangularHandle(0.5f, 0.5f);
	for (float i = 0; i < 8; i++)
	{
		trans = Transform(Vector2(i, -0.5f), 3.14f);
		objs[ctr] = StaticObject(trans, handle);
		trans = Transform(Vector2(i, 7.5f), 3.14f);
		objs[ctr + 1] = StaticObject(trans, handle);
		trans = Transform(Vector2(-0.5f, i), 3.14f / 2);
		objs[ctr + 2] = StaticObject(trans, handle);
		trans = Transform(Vector2(7.5f, i), 3.14f / 2);
		objs[ctr + 3] = StaticObject(trans, handle);
		ctr = ctr + 4;
	}

	//Separator
	handle.m_material = wallMaterial;
	handle.m_mesh = cubeHandle;
	handle.SetUniformScale(1);
	handle.m_yPos = 0;
	handle.m_collider = ColliderManager::get().GetRectangularHandle(0.5f, 0.5f);
	for (float i = 0; i < 5; i++ )
	{
		trans = Transform(Vector2(1.5f + i, 3.5f), 3.14f);
		objs[ctr] = StaticObject(trans, handle);
		trans = Transform(Vector2(3.5f, 1.5f+i), 3.14f / 2);
		objs[ctr + 1] = StaticObject(trans, handle);
		ctr = ctr +2;
	}
	
	m_staticObjectCount = floor + wall + sept;
	m_staticObjs = new StaticObject[m_staticObjectCount];
	memcpy(m_staticObjs, objs, m_staticObjectCount * sizeof(StaticObject));

    // Add player
    handle.m_yPos = 0;
    handle.m_material = playerMaterial;
    handle.m_mesh = cylinderHandle;
    handle.m_collider = ColliderManager::get().GetCircleHandle(.125f);
    handle.SetUniformScale(1);
	handle.m_scale[0] = 0.5;
	handle.m_scale[2] = 0.5;
    Vector2 pos(0, 0);

	m_entityCount = 3;
	m_entities = new EntitySpawnInfo[m_entityCount];
	for (int i = 0; i < m_entityCount; i++)
	{
		handle.m_material = i % 2;
		m_entities[i].m_handle = handle;
		m_entities[i].m_initialTime = 0;
		m_entities[i].m_maxImages = 2048;
		m_entities[i].m_maxPhenomena = 100;
		m_entities[i].m_startingPos = Transform(pos.Rotate(i * 6.28f / m_entityCount), -i * 6.28f / m_entityCount);
		m_entities[i].m_action.m_deploymentTime = .1f;
		m_entities[i].m_action.m_duration = .1f;
		m_entities[i].m_action.m_phenomenaType = i;
	}
    

    // Initialize phenomena
    m_phenomenaTypes = new PhenomenaPrototype[3];
    HandleObject bulletHandle;
    bulletHandle.m_material = 1;
    bulletHandle.m_mesh = 0;
    bulletHandle.m_collider = ColliderManager::get().GetCircleHandle(.000125f);
    bulletHandle.SetUniformScale(.5f);
    m_phenomenaTypes[0].m_handle = bulletHandle;
    m_phenomenaTypes[0].m_period = 1;
    m_phenomenaTypes[0].m_range = 10;
	bulletHandle.m_mesh = 1;
	m_phenomenaTypes[1].m_handle = bulletHandle;
	m_phenomenaTypes[1].m_period = 1;
	m_phenomenaTypes[1].m_range = 10;
	bulletHandle.m_mesh = 2;
	m_phenomenaTypes[2].m_handle = bulletHandle;
	m_phenomenaTypes[2].m_period = 1;
	m_phenomenaTypes[2].m_range = 10;

    m_pTypeCount = 3;

    //Initiating lighting
#ifdef CLIENT
    m_lightList = new Light[3];

    Light directLight, spotLight, pointLight;

    directLight.Type = LIGHT_TYPE_DIRECTIONAL;
    directLight.Direction = DirectX::XMFLOAT3(-1, -1, 0);
    directLight.Color = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f);
    directLight.DiffuseIntensity = 1.0f;
    directLight.AmbientIntensity = .3f;//0.4f;

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

    m_lightList[0] = directLight;
    m_lightList[1] = pointLight;
    m_lightList[2] = spotLight;

    m_lightCount = 3;
#endif // CLIENT

}


ArenaLevel::~ArenaLevel()
{
}
