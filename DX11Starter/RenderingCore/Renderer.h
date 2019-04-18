#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

#include "Mesh.h"
#include "Lights.h"
#include "DrawGroup.h"
#include "Material.h"
#include "UIManager.h"

// Contains the core rendering logic
class Renderer
	: public DXCore
{
	SimplePixelShader* m_ps;
	SimpleVertexShader* m_vs;
	ID3D11SamplerState* m_sampler;

public:
	Renderer(HINSTANCE hInstance);
	~Renderer();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void InitializeShaders();
	
	// Render Controls
	void Begin();
	void End();
	//void DrawScene(SceneGraph* scenegraph, float time);
	void RenderGroup(DrawGroup& drawGroup);

	// Render logic
private:
	void RenderVisibleEntity(DrawItem& entity, Camera& camera, Light* lights, int lightCount);
	void Render(SimplePixelShader* ps, SimpleVertexShader* vs, Material* mat, ID3D11SamplerState* sampler, DirectX::XMFLOAT4X4& transform, Mesh* mesh, Camera& camera, Light* lights, int lightCount);

};

