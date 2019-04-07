#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Entity.h"
#include "Camera.h"

// Manager includes
#include "ResourceManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Lights.h"
 
#include "ServerSceneGraph.h"
#include "ClientHelper.h"

//#include "ServerSceneGraph.h"
//#include "Vector2.h"
 

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadTextures();
	void LoadShaders();
	void InitializeCamera();
	void CreateBasicGeometry();

	// Render logic
	void Render(Material* mat, DirectX::XMFLOAT4X4& transform, int meshHandle);
	void RenderEntity(Entity& entity);
	void RenderObjectAtPos(HandleObject& handle, Transform trans);
	void RenderLerpObject(HandleObject& handle, TimeInstableTransform trans, float t);
	void RenderPhantoms(TemporalEntity& phantom, float t);

	// Networking
	static Game* game;
	static void NetworkRequest(Buffer& buffer);
	static void NetworkData(Buffer& buffer);

	// The matrices to go from model space to screen space
	Camera camera;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	float time = 0;
	bool reversed = false;

	float timeShot = -1;

	// Tracks all entities
	std::vector<DirectionalLight> lightList;

	// Managers
	ResourceManager<SimpleVertexShader*> vertexShaderManager;
	ResourceManager<SimplePixelShader*> pixelShaderManager;

	ResourceManager<ID3D11SamplerState*> samplerManager;
	
	ServerSceneGraph* sceneGraph;
	ClientHelper connection;
};

