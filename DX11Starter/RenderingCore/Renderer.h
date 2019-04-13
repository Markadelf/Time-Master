#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

#include "Mesh.h"
#include "Lights.h"
#include "SceneGraph.h"
#include "DrawGroup.h"
#include "Material.h"

// Contains the core rendering logic
class Renderer
	: public DXCore
{
	SimplePixelShader* m_ps;
	SimpleVertexShader* m_vs;
	ID3D11SamplerState* m_sampler;
	
	// Tracks all entities
	std::vector<DirectionalLight> m_lightList;

public:
	Renderer(HINSTANCE hInstance);
	~Renderer();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void InitializeShaders();

	// Accessors
	std::vector<DirectionalLight>* GetLights();

	// Render Controls
	void Begin();
	void End();
	//void DrawScene(SceneGraph* scenegraph, float time);
	void RenderGroup(DrawGroup& drawGroup);

	// Render logic
private:
	void Render(SimplePixelShader* ps, SimpleVertexShader* vs, ID3D11ShaderResourceView* texture, ID3D11SamplerState* sampler, DirectX::XMFLOAT4X4& transform, Mesh* mesh, Camera& camera);
	//void Render(Material* mat, DirectX::XMFLOAT4X4& transform, int meshHandle);
	void RenderVisibleEntity(DrawItem& entity, Camera& camera);
	//void RenderObjectAtPos(HandleObject& handle, Transform trans);
	//void RenderLerpObject(HandleObject& handle, TimeInstableTransform trans, float t);
	//void RenderPhantoms(TemporalEntity& phantom, float t);

};

