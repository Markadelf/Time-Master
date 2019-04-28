#include "ArenaLevel.h"
#include "StaticObject.h"

ArenaLevel::ArenaLevel()
{

    int floorMaterial = 2;
    int woodMaterial = 1;
    int playerMaterial = 0;

    int coneHandle = 0;
    int cubeHandle = 1;
    int cylinderHandle = 2;
    int sphereHandle = 3;
    int duckHandle = 4;

    // Add static objects to scene graph
    const int div = 20;
    StaticObject objs[div + 2];
    Vector2 right = Vector2(5, 0);
    HandleObject handle;
    handle.m_material = woodMaterial;
    handle.m_mesh = cubeHandle;
    handle.m_scale[2] = 2;
    handle.m_collider = ColliderManager::get().GetRectangularHandle(1, 2);

    Transform trans;
    for (size_t i = 0; i < div; i++)
    {
        trans = Transform(right.Rotate(6.28f / div * i), -6.28f / div * i);
        objs[i] = StaticObject(trans, handle);
    }
    handle.m_material = floorMaterial;
    handle.m_mesh = cylinderHandle;
    handle.SetUniformScale(1);
    handle.m_collider = ColliderManager::get().GetCircleHandle(.5f);
    //handle.m_scale[2] = 1;

    trans = Transform(Vector2(), 0);
    objs[div] = StaticObject(trans, handle);

    // Add floor
    handle.m_collider = Colliders2D::ColliderHandle();
    handle.m_yPos = -1;
    handle.m_scale[0] = 10;
    handle.m_scale[2] = 10;
    objs[div + 1] = StaticObject(trans, handle);

    m_staticObjectCount = div + 2;
    m_staticObjs = new StaticObject[m_staticObjectCount];
    memcpy(m_staticObjs, objs, m_staticObjectCount * sizeof(StaticObject));

    // Add player
    handle.m_yPos = 0;
    handle.m_material = playerMaterial;
    handle.m_mesh = cubeHandle;
    handle.m_collider = ColliderManager::get().GetCircleHandle(.25f);
    handle.SetUniformScale(1);
    Vector2 pos(0, -3);

    m_entities = new EntitySpawnInfo[1];
    m_entityCount = 1;
    m_entities[0].m_handle = handle;
    m_entities[0].m_initialTime = 0;
    m_entities[0].m_maxImages = 2048;
    m_entities[0].m_maxPhenomena = 100;
    m_entities[0].m_startingPos = Transform(pos, 0);
    m_entities[0].m_action.m_deploymentTime = .1f;
    m_entities[0].m_action.m_duration = .3f;
    m_entities[0].m_action.m_phenomenaType = 0;


    // Initialize phenomena
    m_phenomenaTypes = new PhenomenaPrototype[1];
    HandleObject bulletHandle;
    bulletHandle.m_material = 1;
    bulletHandle.m_mesh = 3;
    bulletHandle.m_collider = ColliderManager::get().GetCircleHandle(.25f);
    bulletHandle.SetUniformScale(.5f);
    m_phenomenaTypes[0].m_handle = bulletHandle;
    m_phenomenaTypes[0].m_period = 1;
    m_phenomenaTypes[0].m_range = 10;

    m_pTypeCount = 1;

    //Initiating lighting
#ifdef CLIENT
    m_lightList = new Light[3];

    Light directLight, spotLight, pointLight;

    directLight.Type = LIGHT_TYPE_DIRECTIONAL;
    directLight.Direction = DirectX::XMFLOAT3(-1, -1, 0);
    directLight.Color = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f);
    directLight.DiffuseIntensity = 1.0f;
    directLight.AmbientIntensity = 0;//0.4f;

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
