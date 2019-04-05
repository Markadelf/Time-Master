#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

#include "Camera.h"
#include "Mesh.h"
#include "Lights.h"
#include "Entity.h"
#include "ServerSceneGraph.h"
#include "Material.h"

// Contains the core rendering logic
class Renderer
	: public DXCore
{
	// Camera used for rendering
	Camera m_currentView;
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
	void OnResize();
	void InitializeCamera();
	void InitializeShaders();

	// Accessors
	Camera* GetCamera();
	std::vector<DirectionalLight>* GetLights();

	// Render Controls
	void Begin();
	void End();
	void DrawScene(ServerSceneGraph* scenegraph, float time);

	// Render logic
private:
	void Render(SimplePixelShader* ps, SimpleVertexShader* vs, ID3D11ShaderResourceView* texture, ID3D11SamplerState* sampler, DirectX::XMFLOAT4X4& transform, Mesh* mesh);
	void Render(Material* mat, DirectX::XMFLOAT4X4& transform, int meshHandle);
	void RenderEntity(Entity& entity);
	void RenderObjectAtPos(HandleObject& handle, Transform trans);
	void RenderLerpObject(HandleObject& handle, TimeInstableTransform trans, float t);
	void RenderPhantoms(TemporalEntity& phantom, float t);

};

