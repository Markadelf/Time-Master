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
	const float D_LIGHT_SHADOW_DISTANCE = 10;

	// Shaders
	SimplePixelShader* m_ps;
	SimpleVertexShader* m_vs;
	SimpleVertexShader* m_shadowVS;

	// Smapler States
	ID3D11SamplerState* m_sampler;
	ID3D11SamplerState* m_shadowSampler;

	// Rasterizer states
	ID3D11RasterizerState* m_shadowRasterizer;

	//Blend variables
	SimplePixelShader* m_blendPS;
	ID3D11RasterizerState* m_blendRasterizer;
	ID3D11BlendState* m_blendState;
	ID3D11DepthStencilState* m_blendDepthState;

	int shadowMapSize;

	// Items neccesary to use a single shadow map
	// TODO: Consider having more than one shadow map
	ID3D11DepthStencilView* m_shadowDSV;
	ID3D11ShaderResourceView* m_shadowSRV;
	DirectX::XMFLOAT4X4 m_shadowViewMatrix;
	DirectX::XMFLOAT4X4 m_shadowProjectionMatrix;

	//Skybox: 
	std::vector<Mesh*> meshes;
	ID3D11ShaderResourceView* m_skySRV;
	SimpleVertexShader* m_skyVS;
	SimplePixelShader* m_skyPS;
	ID3D11RasterizerState* m_skyRasterState;
	ID3D11DepthStencilState* m_skyDepthState;

	//Emitter stuff
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;
	ID3D11RasterizerState* particleDebugRasterState;

	void InitializeEmitters();

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
    // Wipes the screen and sets default states
	void Begin();
    // Displays the render target, activating the swap chain
	void End();
    // Renders a collection of commands at once
	void RenderGroup(DrawGroup& drawGroup);
    // Renders the sky
	void DrawSky(Camera& camera);
	
	//Rendering transperant objects


	//Emitter System
	void RenderEmitterSystem(EmitterDrawInfo info, float currentTime, Camera& camera);

	// Render logic
private:
    // Renders a draw item without concern for opacity
	void RenderVisibleEntity(DrawItem& entity, Camera& camera, Light* lights, int lightCount);
	//Renders a transperant entity
	void RenderTransperentEntity(DrawItem& entity, Camera& camera, Light* lights, int lightCount,float transparency);

    // Renders an object with the given parameters
	void Render(SimplePixelShader* ps, SimpleVertexShader* vs, Material* mat, ID3D11SamplerState* sampler, DirectX::XMFLOAT4X4& transform, Mesh* mesh, Camera& camera, Light* lights, int lightCount, float transparency=1);
	// Renders a mesh to a shadow map
    void RenderDepth(DirectX::XMFLOAT4X4& transform, Mesh* mesh);

};

