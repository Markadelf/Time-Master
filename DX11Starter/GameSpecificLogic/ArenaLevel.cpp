#include "ArenaLevel.h"
#include "StaticObject.h"

ArenaLevel::ArenaLevel()
{
    const float pi = 3.14159265358979323846f;
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
	const int floor = 15;
	const int wall = 32;
	const int sept = 10;

	const int total = floor * floor + wall + sept * 4 + 1;
	StaticObject objs[total];
	Transform trans;

	//Floor	
	handle.m_material = floorMaterial;
	handle.m_mesh = cubeHandle;
	handle.SetUniformScale(1);
	handle.m_scale[1] = 0.1f;
	handle.m_yPos = -0.5f;
	int ctr = 0;
	for (float i = 0; i < floor; i++)
	{
		for (float j = 0; j < floor; j++)
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
	handle.m_scale[2] = 0.5f;
	handle.m_yPos = 0;
	handle.m_scale[0] = floor / 8.f;
	float halfWidth = handle.m_scale[0] / 2;
	float thick = handle.m_scale[2];
	handle.m_collider = ColliderManager::get().GetRectangularHandle(halfWidth, thick / 2);
	for (int i = 0; i < wall / 4; i++)
	{
		float dist = halfWidth + i * handle.m_scale[0];
		trans = Transform(Vector2(dist, .5f), pi);
		objs[ctr] = StaticObject(trans, handle);
		trans = Transform(Vector2(dist, floor -.5f), pi);
		objs[ctr + 1] = StaticObject(trans, handle);
		trans = Transform(Vector2(.5f, dist), pi / 2);
		objs[ctr + 2] = StaticObject(trans, handle);
		trans = Transform(Vector2(floor - .5f, dist), pi / 2);
		objs[ctr + 3] = StaticObject(trans, handle);
		ctr = ctr + 4;
	}

	//Separator
	handle.m_material = wallMaterial;
	handle.m_mesh = cubeHandle;
	float dist = 1.f;
	handle.SetUniformScale(dist);
	//handle.m_scale[0] = 1.5f;
	handle.m_yPos = -.5f + dist / 2;
	handle.m_collider = ColliderManager::get().GetRectangularHandle(dist / 2, dist / 2);

	float floor_4 = floor / 4.f;

	Vector2 pivot[] = { 
		Vector2(floor_4, floor_4),
		Vector2(floor_4 * 3, floor_4),
		Vector2(floor_4, floor_4 * 3),
		Vector2(floor_4 * 3, floor_4 * 3),
	};

	for (size_t i = 0; i < 4; i++)
	{
		for (float j = -2; j <= 2; j++)
		{
			trans = Transform(pivot[i] + Vector2(j * dist, 0), pi);
			objs[ctr] = StaticObject(trans, handle);
			trans = Transform(pivot[i] + Vector2(0, j * dist), pi / 2);
			objs[ctr + 1] = StaticObject(trans, handle);
			ctr = ctr + 2;
		}
	}
	
	// Center
	handle.SetUniformScale(dist * 2);
	handle.m_scale[1] = dist;
	//handle.m_scale[0] = 1.5f;
	handle.m_collider = ColliderManager::get().GetRectangularHandle(dist, dist);
	
	Vector2 center(floor / 2.f, floor / 2.f);
	trans = Transform(center, 0);
	objs[ctr] = StaticObject(trans, handle);


	m_staticObjectCount = total;
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
    Vector2 pos(floor / 3, 0);

	m_entityCount = 2;
	m_entities = new EntitySpawnInfo[m_entityCount];
	for (int i = 0; i < m_entityCount; i++)
	{
		handle.m_material = i % 2;
		m_entities[i].m_handle = handle;
		m_entities[i].m_initialTime = 0;
		m_entities[i].m_maxImages = 2048;
		m_entities[i].m_maxPhenomena = 100;
		m_entities[i].m_startingPos = Transform(center + pos.Rotate(i * 2 * pi / m_entityCount), -pi / 2 + i * 2 * pi / m_entityCount);
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
    m_lightList = new Light[1];

    Light directLight, spotLight, pointLight;

    directLight.Type = LIGHT_TYPE_DIRECTIONAL;
    directLight.Direction = DirectX::XMFLOAT3(-1, -1, 0);
    directLight.Color = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f);
    directLight.DiffuseIntensity = 1.0f;
    directLight.AmbientIntensity = .3f;//0.4f;

    pointLight.Type = LIGHT_TYPE_POINT;
    pointLight.Position = DirectX::XMFLOAT3(center.m_x, 0, center.m_y);
    pointLight.Direction = DirectX::XMFLOAT3(1, 1, 0);
    pointLight.Range = 10.0f;
    pointLight.Color = DirectX::XMFLOAT3(1, 0, 1);
    pointLight.DiffuseIntensity = 1.0f;
    pointLight.AmbientIntensity = 0.0f;

    spotLight.Type = LIGHT_TYPE_SPOT;
    spotLight.Position = DirectX::XMFLOAT3(center.m_x, 1, center.m_y);
    spotLight.Direction = DirectX::XMFLOAT3(0, -1, 0);

    spotLight.Range = 20.0f;
    spotLight.Color = DirectX::XMFLOAT3(1, 0, 1);
    spotLight.SpotFalloff = 25.0f;
    spotLight.DiffuseIntensity = 1.0f;
    spotLight.AmbientIntensity = 0.1f;

    m_lightList[0] = directLight;
    //m_lightList[1] = pointLight;
    //m_lightList[2] = spotLight;

    m_lightCount = 1;
#endif // CLIENT

}


ArenaLevel::~ArenaLevel()
{
}
