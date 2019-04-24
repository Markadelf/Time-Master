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
	m_renderer.SetDraw(SDraw, SOnResize);
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

	delete clientInterface;
	//delete inputManager;
	
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
	// initialize the input handler
    //inputManager = new GameInput();
	LoadUI();
	m_renderer.OnResize();
}

void Game::LoadTextures()
{

	//ID3D11ShaderResourceView* image;
	// Add if successful

	ID3D11Device* device = m_renderer.GetDevice();
	ID3D11DeviceContext* context = m_renderer.GetContext();

	AssetManager::get().LoadTexture(L"Textures/paint_albedo.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/paint_roughness.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/wood_albedo.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/wood_roughness.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/floor_albedo.png", device, context);
	AssetManager::get().LoadTexture(L"Textures/floor_roughness.png", device, context);
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

	//For now shinniness is being handled in Assetmanager.Will move it to the material once we have everythin up and running with latest renderer.
	AssetManager::get().LoadMaterial(0, 0, "PLAYER3", "Textures/paint_albedo.png", "Textures/paint_roughness.png");
	AssetManager::get().LoadMaterial(0, 0, "WOOD", "Textures/wood_albedo.png", "Textures/wood_roughness.png");
	AssetManager::get().LoadMaterial(0, 0, "FLOOR", "Textures/floor_albedo.png", "Textures/floor_roughness.png");
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

	clientInterface = new ClientManager();

	clientInterface->Init();
}

void Game::LoadUI()
{
	UIManager::get().SetContext(m_renderer.GetContext());
	int graphID = UIManager::get().MakeGraph();
	UIGraph& graph = UIManager::get().GetGraph(graphID);
	UIManager::get().SetGraphActiveInFront(graphID);
	
	UIElement element;
	element.m_transform.m_size = Vector2(.25f, .25f);
	element.m_transform.m_anchor = Vector2(0, 0);
	element.m_transform.m_pivot = Vector2(0, 0);
	element.m_color = DirectX::XMFLOAT4(1, 1, 1, .5f);
	element.m_textureHandle = 0;
	element.m_transform.m_parent = graph.AddItem(element);
	element.m_transform.m_size = Vector2(.5f, .5f);
	element.m_transform.m_anchor = Vector2(0, 0);
	element.m_transform.m_pivot = Vector2(0, 0);
	element.m_color = DirectX::XMFLOAT4(1, 0, 0, 1);
	element.m_transform.m_parent = graph.AddItem(element);
	element.m_transform.m_anchor = Vector2(1, 1);
	element.m_transform.m_pivot = Vector2(1, 1);
	element.m_color = DirectX::XMFLOAT4(0, 1, 0, 1);
	element.m_transform.m_parent = graph.AddItem(element);
	element.m_transform.m_anchor = Vector2(0, 0);
	element.m_transform.m_pivot = Vector2(0, 0);
	element.m_color = DirectX::XMFLOAT4(0, 0, 1, 1);
	element.m_transform.m_parent = graph.AddItem(element);
}



// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		m_renderer.Quit();
	clientInterface->Update(deltaTime);
}

/*
/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Update ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
void Game::acquireInput()
{
	inputManager->acquireInput();

	// act on user input
	for (auto x : inputManager->activeKeyMap)
	{
		switch (x.first)
		{
		case input::GameCommands::Quit:
			m_renderer.Quit();
			break;

		case input::GameCommands::showFPS:
			printf("TEST FPS");
			break;

		case input::GameCommands::Shoot:
			printf("SHOOTING");
			break;

		case input::GameCommands::PlayerMoveForward:
			printf("W");
			break;

		case input::GameCommands::PlayerMoveBack:
			printf("S");
			break;
		case input::GameCommands::PlayerMoveLeft:
			printf("A");
			break;
		case input::GameCommands::PlayerMoveRight:
			printf("D");
		}
	}
}
*/

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

	m_renderer.RenderGroup(clientInterface->GetDrawGroup());
	UIManager::get().Render();

	m_renderer.End();
}

void Game::OnResize(int width, int height)
{
	clientInterface->GetDrawGroup().m_camera.SetAspectRatio((float)width / height);
	UIManager::get().SetScreenDimensions(width, height);
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

	clientInterface->GetPlayer().Rotate(dX / 180.f);

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

#pragma region Static Callbacks
void Game::SDraw(float deltaTime, float totalTime)
{
	GameInstance->Draw(deltaTime, totalTime);
}

void Game::SOnResize(int width, int height)
{
	GameInstance->OnResize(width, height);
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
#pragma endregion