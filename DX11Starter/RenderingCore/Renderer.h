#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

#include "Mesh.h"
#include "Lights.h"
#include "DrawGroup.h"
#include "Material.h"

// Contains the core rendering logic
class Renderer
	: public DXCore
{
	// Shaders
	SimplePixelShader* m_ps;
	SimpleVertexShader* m_vs;
	SimpleVertexShader* m_shadowVS;

	// Smapler States
	ID3D11SamplerState* m_sampler;
	ID3D11SamplerState* m_shadowSampler;

	// Rasterizer states
	ID3D11RasterizerState* m_shadowRasterizer;

	int shadowMapSize;

	// Temporary shadow qualities
	// TODO: Move somewhere else
	ID3D11DepthStencilView* m_shadowDSV;
	ID3D11ShaderResourceView* m_shadowSRV;
	DirectX::XMFLOAT4X4 m_shadowViewMatrix;
	DirectX::XMFLOAT4X4 m_shadowProjectionMatrix;

public:
	Renderer(HINSTANCE hInstance);
	~Renderer();

private:
	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void InitializeShaders();
	void InitializeShadowMaps();

public:
	// Render Controls
	void Begin();
	void End();
	//void DrawScene(SceneGraph* scenegraph, float time);
	void RenderGroup(DrawGroup& drawGroup);

	// Render logic
private:
	void RenderVisibleEntity(DrawItem& entity, Camera& camera, Light* lights, int lightCount);
	void Render(SimplePixelShader* ps, SimpleVertexShader* vs, Material* mat, ID3D11SamplerState* sampler, DirectX::XMFLOAT4X4& transform, Mesh* mesh, Camera& camera, Light* lights, int lightCount);
	void RenderToShadowMap(DirectX::XMFLOAT4X4& transform, Mesh* mesh);

};

