#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

#include "Camera.h"
#include "Mesh.h"
#include "Lights.h"

// Contains the main rendering logic
class Renderer
	: public DXCore
{
	// Camera used for rendering
	Camera m_currentView;
	
	// Tracks all entities
	std::vector<DirectionalLight> m_lightList;

public:
	Renderer(HINSTANCE hInstance);
	~Renderer();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void InitializeCamera();

	// Accessors
	Camera* GetCamera();
	std::vector<DirectionalLight>* GetLights();

	// Render Controls
	void Begin();
	void End();

	// Render logic
	void Render(SimplePixelShader* ps, SimpleVertexShader* vs, ID3D11ShaderResourceView* texture, ID3D11SamplerState* sampler, DirectX::XMFLOAT4X4& transform, Mesh* mesh);
};

