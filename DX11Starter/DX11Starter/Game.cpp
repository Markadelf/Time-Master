#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include "FilePathHelper.h"
#include <cmath>


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
Game::Game(HINSTANCE hInstance): m_renderer(hInstance)
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
	meshManager.Release();
	materialManager.Release();

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	vertexShaderManager.ReleasePointers();
	pixelShaderManager.ReleasePointers();

	// Clean up my texture pointers
	textureManager.ReleaseDXPointers();
	samplerManager.ReleaseDXPointers();

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

	DirectionalLight light1;
	light1.AmbientColor = DirectX::XMFLOAT4(.3f, .3f, .3f, 1);
	light1.DiffuseColor = DirectX::XMFLOAT4(0, 0, 1, 1);
	light1.Direction = DirectX::XMFLOAT3(1, 20, 10);

	DirectionalLight light2;
	light2.AmbientColor = DirectX::XMFLOAT4(.3f, .3f, .3f, 1);
	light2.DiffuseColor = DirectX::XMFLOAT4(1, 1, 0, 1);
	light2.Direction = DirectX::XMFLOAT3(-1, -10, -20);

	lightList->push_back(light1);
	lightList->push_back(light2);
}

void Game::LoadTextures()
{
	ID3D11ShaderResourceView* image;
	auto device = m_renderer.GetDevice();
	auto context = m_renderer.GetContext();
	// Add if successful
	if (CreateWICTextureFromFile(device, context, FilePathHelper::GetPath(L"Textures/poster.png").c_str(), 0, &image) == 0)
		textureManager.AddResource("Textures/poster.png", image);
	if (CreateWICTextureFromFile(device, context, FilePathHelper::GetPath(L"Textures/player3.png").c_str(), 0, &image) == 0)
		textureManager.AddResource("Textures/player3.png", image);
	if (CreateWICTextureFromFile(device, context, FilePathHelper::GetPath(L"Textures/Wooden.png").c_str(), 0, &image) == 0)
		textureManager.AddResource("Textures/Wooden.png", image);
	if (CreateWICTextureFromFile(device, context, FilePathHelper::GetPath(L"Textures/Stripes.png").c_str(), 0, &image) == 0)
		textureManager.AddResource("Textures/Stripes.png", image);

	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	// Add if successful
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
	auto device = m_renderer.GetDevice();
	auto context = m_renderer.GetContext();
	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	int vHandle = vertexShaderManager.AddResource("V1", vertexShader);

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	int pHandle = pixelShaderManager.AddResource("P1", pixelShader);

	materialManager.AddResource("DEFAULT", Material(vHandle, pHandle, textureManager.GetHandle("Textures/poster.png"), 0));
	materialManager.AddResource("PLAYER3", Material(vHandle, pHandle, textureManager.GetHandle("Textures/player3.png"), 0));
	materialManager.AddResource("WOODEN", Material(vHandle, pHandle, textureManager.GetHandle("Textures/Wooden.png"), 0));
	materialManager.AddResource("STRIPES", Material(vHandle, pHandle, textureManager.GetHandle("Textures/Stripes.png"), 0));
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	auto device = m_renderer.GetDevice();
	auto context = m_renderer.GetContext();
	// Load in the files and get the handles for each from the meshManager
	int coneHandle = meshManager.AddResource("OBJ_Files/cone.obj", Mesh("OBJ_Files/cone.obj", device));

	int cubeHandle = meshManager.AddResource("OBJ_Files/cube.obj", Mesh("OBJ_Files/cube.obj", device));

	int cylinderHandle = meshManager.AddResource("OBJ_Files/cylinder.obj", Mesh("OBJ_Files/cylinder.obj", device));
	int sphereHandle = meshManager.AddResource("OBJ_Files/sphere.obj", Mesh("OBJ_Files/sphere.obj", device));

	int duckHandle = meshManager.AddResource("OBJ_Files/duck.fbx", Mesh("OBJ_Files/duck.fbx", device));

	int matHandle = materialManager.GetHandle("DEFAULT");
	int matHandle2 = materialManager.GetHandle("STRIPES");
	int matHandle3 = materialManager.GetHandle("PLAYER3");
	int matHandle4 = materialManager.GetHandle("WOODEN");

	sceneGraph = new ServerSceneGraph(3, 10, 10);

	// Add static objects to scene graph
	const int div = 20;
	StaticObject objs[div + 1];
	Vector2 right = Vector2(5, 0);
	HandleObject handle;
	handle.m_material = matHandle;
	handle.m_mesh = cubeHandle;
	handle.m_scale[2] = 2;
	handle.m_collider = sceneGraph->GetColliderHandle(Colliders2D::ColliderType::Rectangle, 1, 2);

	for (size_t i = 0; i < div; i++)
	{
		objs[i] = (StaticObject(Transform(right.Rotate(6.28f / div * i), -6.28f / div * i), handle));
	}
	handle.m_material = matHandle4;
	handle.m_mesh = duckHandle;
	handle.SetUniformScale(.01f);
	handle.m_collider = sceneGraph->GetColliderHandle(Colliders2D::ColliderType::Circle, .5f);
	//handle.m_scale[2] = 1;

	objs[div] = (StaticObject(Transform(Vector2(), 0), handle));

	sceneGraph->Init(&objs[0], div + 1);

	handle.m_material = matHandle2;
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
	handle.m_material = matHandle;
	sceneGraph->GetEntity(id)->Initialize(Transform(Vector2(pos.x, pos.z).Rotate(3.14f / 3 * 2), camera->GetYaw()), time, handle);

	// Add another player
	id = sceneGraph->AddEntity(2048, 100);
	handle.m_material = matHandle3;
	sceneGraph->GetEntity(id)->Initialize(Transform(Vector2(pos.x, pos.z).Rotate(-3.14f / 3 * 2), camera->GetYaw()), time, handle);
}

void Game::Render(Material* mat, XMFLOAT4X4& transform, int meshHandle)
{
	m_renderer.Render(*pixelShaderManager.GetResourcePointer(mat->GetPixelShaderHandle()),
		*vertexShaderManager.GetResourcePointer(mat->GetVertexShaderHandle()), 
		*textureManager.GetResourcePointer(mat->GetTextureHandle()), 
		*samplerManager.GetResourcePointer(mat->GetSamplerHandle()), 
		transform,
		meshManager.GetResourcePointer(meshHandle));
}

void Game::RenderEntity(Entity& entity)
{
	Render(materialManager.GetResourcePointer(entity.GetMaterialHandle()), entity.GetTransform(), entity.GetMeshHandle());
}

void Game::RenderObjectAtPos(HandleObject& handle, Transform trans)
{
	XMMATRIX matrix = XMMatrixScaling(handle.m_scale[0], handle.m_scale[1], handle.m_scale[2]);
	matrix = XMMatrixMultiply(matrix, XMMatrixRotationRollPitchYaw(0, trans.GetRot(), 0));
	matrix = XMMatrixMultiply(matrix, XMMatrixTranslation(trans.GetPos().GetX(), 0, trans.GetPos().GetY()));
	XMFLOAT4X4 transform;
	XMStoreFloat4x4(&transform, XMMatrixTranspose(matrix));

	Render(materialManager.GetResourcePointer(handle.m_material), transform, handle.m_mesh);
}

void Game::RenderLerpObject(HandleObject& handle, TimeInstableTransform trans, float t)
{
	RenderObjectAtPos(handle, trans.GetTransform(t));
}

void Game::RenderPhantoms(TemporalEntity& phantom, float t)
{
	int phantoms = phantom.GetImageCount();
	Phantom* phantomBuffer = phantom.GetPhantomBuffer();
	HandleObject handle = phantom.GetHandle();
	for (size_t i = 0; i < phantoms; i++)
	{
		TimeInstableTransform trans = phantomBuffer[i].GetTransform();
		if (trans.GetEndTime() > t && trans.GetStartTime() <= t)
		{
			RenderLerpObject(handle, trans, t);
		}
	}

	int phenomina = phantom.GetPhenominaCount();
	Phenomina* phenominaBuffer = phantom.GetPhenominaBuffer();
	for (size_t i = 0; i < phenomina; i++)
	{
		TimeInstableTransform trans = phenominaBuffer[i].GetTransform();
		if (trans.GetEndTime() > t && trans.GetStartTime() <= t)
		{
			handle = phenominaBuffer[i].GetHandle();
			RenderLerpObject(handle, trans, t);
		}
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
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
			// Add to update list
			//bulletList.push_back(bullet);
			held = true;
		}
	}
	else
	{
		held = false;
	}

	static float tBack = 0;
	static int fBack = 0;
	if (GetAsyncKeyState('T') & 0x8000)
	{
		tBack = time;
		fBack = sceneGraph->GetEntity(0)->GetImageCount();
	}

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		TemporalEntity* e = sceneGraph->GetEntity(0);
		PhenominaHandle reset;
		e->Kill(fBack, tBack, PhenominaHandle(), reset);
		if (e->CheckRevive(PhenominaHandle(0, 0)))
		{
			e->Revive();
		}
		time = e->GetTimeStamp();
		reversed = e->GetReversed();

		XMFLOAT3 pos = cam->GetPosition();
		Transform trans = e->GetTransform();
		Vector2 nPos = trans.GetPos();
		pos.x = nPos.GetX();
		pos.z = nPos.GetY();
		cam->SetPosition(pos);
		cam->SetYaw(trans.GetRot());
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
		//sceneGraph->StackKeyFrame(KeyFrameData(Transform(Vector2(pos.x, pos.z).Rotate(3.14f), camera.GetYaw()), time, 1, false, timeShot));
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
	
	StaticObject* objs;
	int sCount;
	sceneGraph->GetStatics(&objs, sCount);
	for (size_t i = 0; i < sCount; i++)
	{
		RenderObjectAtPos(objs[i].GetHandles(), objs[i].GetTransform());
	}
	int eCount = sceneGraph->GetEntityCount();
	for (size_t i = 0; i < eCount; i++)
	{
		RenderPhantoms(*sceneGraph->GetEntity(i), time);
	}

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
	SetCapture(*m_renderer.GethWnd());
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
	auto cam = m_renderer.GetCamera();
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