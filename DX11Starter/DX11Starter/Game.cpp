#include <cmath>
#include "AssetManager.h"
#include "FilePathHelper.h"
#include "Game.h"
#include "Vertex.h"

Game* Game::GameInstance;

// For the DirectX Math library
using namespace DirectX;
// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance) : m_renderer(hInstance)
{
	GameInstance = this;
	m_renderer.SetDraw(SDraw);
	m_renderer.SetUpdate(SUpdate);
	m_renderer.SetControls(SOnMouseDown, SOnMouseUp, SOnMouseMove, SOnMouseWheel);
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class	
	//AssetManager::get().~AssetManager();
	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	AssetManager::get().ReleaseAllAssetResource();

	delete sceneGraph;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadTextures();
	LoadShaders();
	CreateBasicGeometry();

	std::vector<DirectionalLight>* lightList = m_renderer.GetLights();

	//Initiating lighting
	Light directLight, spotLight, pointLight;

	directLight.Type = LIGHT_TYPE_DIRECTIONAL;
	directLight.Direction = XMFLOAT3(-1, 0, 0);
	directLight.Color = XMFLOAT3(0.8f, 0.8f, 0.8f);
	directLight.DiffuseIntensity = 1.0f;
	directLight.AmbientIntensity = 0.4f;

	pointLight.Type = LIGHT_TYPE_POINT;
	pointLight.Position = XMFLOAT3(-3, -3, 0);
	pointLight.Direction = XMFLOAT3(1, 1, 0);
	pointLight.Range = 20.0f;
	pointLight.Color = XMFLOAT3(1.0f, 0.1, 0);
	pointLight.DiffuseIntensity = 1.0f;
	pointLight.AmbientIntensity = 0.0f;

	spotLight.Type = LIGHT_TYPE_SPOT;
	spotLight.Position = XMFLOAT3(0, 5, 0);
	spotLight.Direction = XMFLOAT3(0, -1, 0);

	spotLight.Range = 20.0f;
	spotLight.Color = XMFLOAT3(1, 0, 1);
	spotLight.SpotFalloff = 25.0f;
	spotLight.DiffuseIntensity = 1.0f;
	spotLight.AmbientIntensity = 0.1f;

	lightList.push_back(directLight);
	lightList.push_back(spotLight);
	lightList.push_back(pointLight);

}

void Game::LoadTextures()
{

	ID3D11ShaderResourceView* image;
	// Add if successful

	ID3D11Device* device = m_renderer.GetDevice();
	ID3D11DeviceContext* context = m_renderer.GetContext();
	
	AssetManager::get().LoadTexture(L"Textures/paint_albedo.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/paint_roughness.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/wood_albedo.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/wood_roughness.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/floor_albedo.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/floor_roughness.png", device, context);

	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	if (device->CreateSamplerState(&desc, &sampler) == 0)
		samplerManager.AddResource("0", sampler);

}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	ID3D11Device* device = m_renderer.GetDevice();
	ID3D11DeviceContext* context = m_renderer.GetContext();

	int pHandle = pixelShaderManager.AddResource("P1", pixelShader);
	
	AssetManager::get().LoadMaterial(vHandle, pHandle, "PLAYER3", "Textures/paint_albedo.png", "Textures/paint_roughness.png");
	AssetManager::get().LoadMaterial(vHandle, pHandle, "WOOD", "Textures/wood_albedo.png", "Textures/wood_roughness.png");
	AssetManager::get().LoadMaterial(vHandle, pHandle, "FLOOR", "Textures/floor_albedo.png", "Textures/floor_roughness.png");
	
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	ID3D11Device* device = m_renderer.GetDevice();
	ID3D11DeviceContext* context = m_renderer.GetContext();
	// Load in the files and get the handles for each from the meshManager
	int coneHandle = AssetManager::get().LoadMesh("OBJ_Files/cone.obj", device);

	int cubeHandle = AssetManager::get().LoadMesh("OBJ_Files/cube.obj", device);

	int cylinderHandle = AssetManager::get().LoadMesh("OBJ_Files/cylinder.obj", device);
	int sphereHandle = AssetManager::get().LoadMesh("OBJ_Files/sphere.obj", device);

	int duckHandle = AssetManager::get().LoadMesh("OBJ_Files/duck.fbx", device);

	int floorMaterial = AssetManager::get().GetMaterialHandle("FLOOR");
	int woodMaterial  = AssetManager::get().GetMaterialHandle("WOOD");
	int playerMaterial = AssetManager::get().GetMaterialHandle("PLAYER3");


	sceneGraph = new ServerSceneGraph(3, 10, 10);

	// Add static objects to scene graph
	const int div = 20;
	StaticObject objs[div + 1];
	Vector2 right = Vector2(5, 0);
	HandleObject handle;
	handle.m_material = woodMaterial;
	handle.m_mesh = cubeHandle;
	handle.m_scale[2] = 2;
	handle.m_collider = sceneGraph->GetColliderHandle(Colliders2D::ColliderType::Rectangle, 1, 2);

	for (size_t i = 0; i < div; i++)
	{
		objs[i] = (StaticObject(Transform(right.Rotate(6.28f / div * i), -6.28f / div * i), handle));
	}
	handle.m_material = playerMaterial;
	handle.m_mesh = duckHandle;
	handle.SetUniformScale(.01f);
	handle.m_collider = sceneGraph->GetColliderHandle(Colliders2D::ColliderType::Circle, .5f);
	//handle.m_scale[2] = 1;

	objs[div] = (StaticObject(Transform(Vector2(), 0), handle));

	sceneGraph->Init(&objs[0], div + 1);

	handle.m_material = floorMaterial;
	handle.m_mesh = cubeHandle;
	handle.m_collider = sceneGraph->GetColliderHandle(Colliders2D::ColliderType::Circle, .25f);
	handle.m_scale[0] = 1;
	handle.m_scale[1] = 1;
	handle.m_scale[2] = 1;
	Camera* camera = m_renderer.GetCamera();
	XMFLOAT3 pos = camera->GetPosition();
	// Add player
	int id = sceneGraph->AddEntity(2048, 100);
	sceneGraph->GetEntity(id)->Initialize(Transform(Vector2(pos.x, pos.z), camera->GetYaw()), time, handle);

	// Add another player
	id = sceneGraph->AddEntity(2048, 100);
	handle.m_material = playerMaterial;
	sceneGraph->GetEntity(id)->Initialize(Transform(Vector2(pos.x, pos.z).Rotate(3.14f / 3 * 2), camera.GetYaw()), time, handle);

	// Add another player
	id = sceneGraph->AddEntity(2048, 100);
	handle.m_material = playerMaterial;
	sceneGraph->GetEntity(id)->Initialize(Transform(Vector2(pos.x, pos.z).Rotate(-3.14f / 3 * 2), camera.GetYaw()), time, handle);
}



// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// TODO: Migrate update game logic somewhere else
	Camera* cam = m_renderer.GetCamera();
	time += deltaTime * (reversed ? -1 : 1);
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		m_renderer.Quit();
	if (GetAsyncKeyState('W') & 0x8000)
	{
		cam->MoveRelative(XMFLOAT3(0, 0, 1 * deltaTime));
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		cam->MoveRelative(XMFLOAT3(1 * deltaTime, 0, 0));
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		cam->MoveRelative(XMFLOAT3(0, 0, -1 * deltaTime));
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		cam->MoveRelative(XMFLOAT3(-1 * deltaTime, 0, 0));
	}
	//Keep camera from moving into walls
	XMFLOAT3 pos = cam->GetPosition();
	Transform trans = Transform(Vector2(pos.x, pos.z), 0);
	if (sceneGraph->PreventCollision(0, trans))
	{
		Vector2 nPos = trans.GetPos();
		pos.x = nPos.GetX();
		pos.z = nPos.GetY();
		cam->SetPosition(pos);
	}

	static bool rHeld = false;
	if (GetAsyncKeyState('R') & 0x8000)
	{
		if (!rHeld)
		{
			reversed = !reversed;
			rHeld = true;
		}
	}
	else
	{
		rHeld = false;
	}
	static bool held = false;
	if (GetAsyncKeyState(' ') & 0x8000)
	{
		if (!held)
		{
			timeShot = time;
			held = true;
		}
	}
	else
	{
		held = false;
	}

	static int activePlayer = 0;
	if (GetAsyncKeyState('1') & 0x8000)
	{
		if (activePlayer != 0)
		{
			TemporalEntity* e = sceneGraph->GetEntity(0);
			time = e->GetTimeStamp();
			reversed = e->GetReversed();

			XMFLOAT3 pos = cam->GetPosition();
			Transform trans = e->GetTransform();
			Vector2 nPos = trans.GetPos();
			pos.x = nPos.GetX();
			pos.z = nPos.GetY();
			cam->SetPosition(pos);
			cam->SetYaw(trans.GetRot());
			activePlayer = 0;
		}
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		if (activePlayer != 1)
		{
			TemporalEntity* e = sceneGraph->GetEntity(1);
			time = e->GetTimeStamp();
			reversed = e->GetReversed();

			XMFLOAT3 pos = cam->GetPosition();
			Transform trans = e->GetTransform();
			Vector2 nPos = trans.GetPos();
			pos.x = nPos.GetX();
			pos.z = nPos.GetY();
			cam->SetPosition(pos);
			cam->SetYaw(trans.GetRot());
			activePlayer = 1;
		}
	}
	if (GetAsyncKeyState('3') & 0x8000)
	{
		if (activePlayer != 2)
		{
			TemporalEntity* e = sceneGraph->GetEntity(2);
			time = e->GetTimeStamp();
			reversed = e->GetReversed();

			XMFLOAT3 pos = cam->GetPosition();
			Transform trans = e->GetTransform();
			Vector2 nPos = trans.GetPos();
			pos.x = nPos.GetX();
			pos.z = nPos.GetY();
			cam->SetPosition(pos);
			cam->SetYaw(trans.GetRot());
			activePlayer = 2;
		}
	}
	static int frame = 0;
	if (frame > 30 || timeShot != -1)
	{
		XMFLOAT3 pos = cam->GetPosition();
		sceneGraph->StackKeyFrame(KeyFrameData(Transform(Vector2(pos.x, pos.z), cam->GetYaw()), time, activePlayer, timeShot != -1, timeShot));
		frame = 0;
		timeShot = -1;
	}
	else
	{
		frame++;
	}
}

void Game::SUpdate(float deltaTime, float totalTime)
{
	GameInstance->Update(deltaTime, totalTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	m_renderer.Begin();

	m_renderer.DrawScene(sceneGraph, time);

	m_renderer.End();
}

void Game::SDraw(float deltaTime, float totalTime)
{
	GameInstance->Draw(deltaTime, totalTime);
}

void Game::SOnMouseDown(WPARAM buttonState, int x, int y)
{
	GameInstance->OnMouseDown(buttonState, x, y);
}

void Game::SOnMouseUp(WPARAM buttonState, int x, int y)
{
	GameInstance->OnMouseUp(buttonState, x, y);
}

void Game::SOnMouseMove(WPARAM buttonState, int x, int y)
{
	GameInstance->OnMouseMove(buttonState, x, y);
}

void Game::SOnMouseWheel(float wheelDelta, int x, int y)
{
	GameInstance->OnMouseWheel(wheelDelta, x, y);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(m_renderer.GethWnd());
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	int dY = (y - prevMousePos.y);
	int dX = (x - prevMousePos.x);

	dX = dX % 10;
	dY = dY % 10;
	Camera* cam = m_renderer.GetCamera();
	cam->SetYaw(cam->GetYaw() + dX / 180.f);
	//camera.SetPitch(camera.GetPitch() + dY / 180.f);

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
Renderer* Game::GetRenderer()
{
	return &m_renderer;
}
#pragma endregion