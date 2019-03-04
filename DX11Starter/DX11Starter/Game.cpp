#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include "FilePathHelper.h"

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
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		// The application's handle
		"DirectX Game",	   	// Text for the window's title bar
		1280,			// Width of the window's client area
		720,			// Height of the window's client area
		true)			// Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
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
	InitializeCamera();
	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectionalLight light1;
	light1.AmbientColor = DirectX::XMFLOAT4(.3f, .3f, .3f, 1);
	light1.DiffuseColor = DirectX::XMFLOAT4(0, 0, 1, 1);
	light1.Direction = DirectX::XMFLOAT3(1, 20, 10);

	DirectionalLight light2;
	light2.AmbientColor = DirectX::XMFLOAT4(.3f, .3f, .3f, 1);
	light2.DiffuseColor = DirectX::XMFLOAT4(1, 1, 0, 1);
	light2.Direction = DirectX::XMFLOAT3(-1, -10, -20);

	lightList.push_back(light1);
	lightList.push_back(light2);
}

void Game::LoadTextures()
{
	ID3D11ShaderResourceView* image;
	// Add if successful
	if(CreateWICTextureFromFile(device, context, FilePathHelper::GetPath(L"Textures/poster.png").c_str(), 0, &image) == 0)
		textureManager.AddResource("Textures/poster.png", image);
	if (CreateWICTextureFromFile(device, context, FilePathHelper::GetPath(L"Textures/stripes.png").c_str(), 0, &image) == 0)
		textureManager.AddResource("Textures/stripes.png", image);

	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	// Add if successful
	if(device->CreateSamplerState(&desc, &sampler) == 0)
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
    SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	int vHandle = vertexShaderManager.AddResource("V1", vertexShader);

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	int pHandle = pixelShaderManager.AddResource("P1", pixelShader);

	materialManager.AddResource("DEFAULT", Material(vHandle, pHandle, textureManager.GetHandle("Textures/poster.png"), 0));
	materialManager.AddResource("STRIPES", Material(vHandle, pHandle, textureManager.GetHandle("Textures/stripes.png"), 0));
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::InitializeCamera()
{
	camera.SetPosition(XMFLOAT3(0, 0, -5));
	camera.SetYaw(0);
	camera.SetAspectRatio((float)width / height);
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Load in the files and get the handles for each from the meshManager
	int coneHandle = meshManager.AddResource("OBJ Files/cone.obj", Mesh("OBJ Files/cone.obj", device));

	int cubeHandle = meshManager.AddResource("OBJ Files/cube.obj", Mesh("OBJ Files/cube.obj", device));

	int cylinderHandle = meshManager.AddResource("OBJ Files/cylinder.obj", Mesh("OBJ Files/cylinder.obj", device));

	int matHandle = materialManager.GetHandle("DEFAULT");
	int matHandle2 = materialManager.GetHandle("STRIPES");


	// Make a bunch of entities
	entityList.push_back(Entity(coneHandle, matHandle, DirectX::XMFLOAT3(3, 0, 0), DirectX::XMFLOAT3(.05f, .05f, .05f), DirectX::XMFLOAT4(0, 0, 0, 1)));
	entityList.push_back(Entity(cubeHandle, matHandle2, DirectX::XMFLOAT3(-2, 0, 0)));
	entityList.push_back(Entity(cubeHandle, matHandle, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(1, 1, 1), DirectX::XMFLOAT4(0, 0, 1, 0)));
	entityList.push_back(Entity(coneHandle, matHandle2, DirectX::XMFLOAT3(3, 0, 0), DirectX::XMFLOAT3(.15f, .15f, .15f), DirectX::XMFLOAT4(0, 0, 0, 1)));
	entityList.push_back(Entity(cylinderHandle, matHandle2, DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT3(1, 1, 1), DirectX::XMFLOAT4(0, 0, 1, 0)));
	entityList.push_back(Entity(cylinderHandle, matHandle, DirectX::XMFLOAT3(2, 1, 0), DirectX::XMFLOAT3(1, 1, 1), DirectX::XMFLOAT4(0, 0, 0, 1)));

}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	camera.SetAspectRatio((float)width / height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
	if (GetAsyncKeyState('W') & 0x8000) 
	{
		camera.MoveRelative(XMFLOAT3(0, 0, 1 * deltaTime));
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		camera.MoveRelative(XMFLOAT3(1 * deltaTime, 0, 0));
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		camera.MoveRelative(XMFLOAT3(0, 0, -1 * deltaTime));
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		camera.MoveRelative(XMFLOAT3(-1 * deltaTime, 0, 0));
	}
	if (GetAsyncKeyState(' ') & 0x8000)
	{
		camera.MoveRelative(XMFLOAT3(0, 1 * deltaTime, 0));
	}
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	{
		camera.MoveRelative(XMFLOAT3(0, -1 * deltaTime, 0));
	}
	entityList[1].Move(XMFLOAT3(1 * deltaTime, 0, 0));
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);


	for (int i = 0; i < entityList.size(); i++) {
		Material* mat = materialManager.GetResourcePointer(entityList[i].GetMaterialHandle());
		SimplePixelShader* pixelShader = *pixelShaderManager.GetResourcePointer(mat->GetPixelShaderHandle());
		SimpleVertexShader* vertexShader = *vertexShaderManager.GetResourcePointer(mat->GetVertexShaderHandle());

		// Send data to shader variables
		//  - Do this ONCE PER OBJECT you're drawing
		//  - This is actually a complex process of copying data to a local buffer
		//    and then copying that entire buffer to the GPU.  
		//  - The "SimpleShader" class handles all of that for you.
		vertexShader->SetMatrix4x4("world", entityList[i].GetTransform());
		vertexShader->SetMatrix4x4("view", camera.GetView());
		vertexShader->SetMatrix4x4("projection", camera.GetProjection());

		// Once you've set all of the data you care to change for
		// the next draw call, you need to actually send it to the GPU
		//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
		vertexShader->CopyAllBufferData();
		pixelShader->SetInt("lightAmount", (int)lightList.size());
		// Only copies first ten as the size is fixed on the shader. Subtracting the pad value is necessary because the 
		pixelShader->SetData("light",  &lightList[0], sizeof(DirectionalLight) * 10 - DirectionalLight::PAD);
		pixelShader->SetShaderResourceView("diffuseTexture", *textureManager.GetResourcePointer(mat->GetTextureHandle()));
		pixelShader->SetSamplerState("basicSampler", *samplerManager.GetResourcePointer(mat->GetSamplerHandle()));
		pixelShader->CopyAllBufferData();

		// Set the vertex and pixel shaders to use for the next Draw() command
		//  - These don't technically need to be set every frame...YET
		//  - Once you start applying different shaders to different objects,
		//    you'll need to swap the current shaders before each draw
		vertexShader->SetShader();
		pixelShader->SetShader();

		// Set buffers in the input assembler
		//  - Do this ONCE PER OBJECT you're drawing, since each object might
		//    have different geometry.
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		Mesh* mesh = meshManager.GetResourcePointer(entityList[i].GetMeshHandle());

		ID3D11Buffer* vBuffer = mesh->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
		context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		// Finally do the actual drawing
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		context->DrawIndexed(
			mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
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
	SetCapture(hWnd);
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

	camera.SetYaw(camera.GetYaw() + dX / 180.f);
	camera.SetPitch(camera.GetPitch() + dY / 180.f);

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
#pragma endregion