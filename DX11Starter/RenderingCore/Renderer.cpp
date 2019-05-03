#include "Renderer.h"
#include "Vertex.h"
#include "FilePathHelper.h"
#include <cmath>
#include "AssetManager.h"
#include "DDSTextureLoader.h"

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
Renderer::Renderer(HINSTANCE hInstance)
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

	m_ps = nullptr;
	m_vs = nullptr;
	m_sampler = nullptr;
	m_blendPS = nullptr;
	m_skyPS = nullptr;
	m_skyVS = nullptr;

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Renderer::~Renderer()
{
	delete m_ps;
	delete m_vs;
	delete m_shadowVS;
	delete m_skyPS;
	delete m_skyVS;
	delete m_blendPS;

	m_skySRV->Release();
	m_skyRasterState->Release();
	m_skyDepthState->Release();
	m_sampler->Release();


	m_shadowSampler->Release();
	m_shadowRasterizer->Release();

	m_shadowDSV->Release();
	m_shadowSRV->Release();

	m_blendState->Release();
	m_blendDepthState->Release();
	m_blendRasterizer->Release();

}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Renderer::Init()
{
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	InitializeShaders();
	InitializeShadowMaps();
	resize(width, height);
	
	//Maximize on Launch
	ShowWindow(Renderer::GethWnd(), SW_MAXIMIZE);
	SetCursorPos((int)(width / 2), (int)(height / 2));
	

	// Load the sky box
	CreateDDSTextureFromFile(device, L"../../Assets/Textures/SunnyCubeMap.dds", 0, &m_skySRV);
		
	// Create the states for the sky
	D3D11_RASTERIZER_DESC rd = {};
	rd.CullMode = D3D11_CULL_FRONT;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.DepthClipEnable = true;
	device->CreateRasterizerState(&rd, &m_skyRasterState);

	D3D11_DEPTH_STENCIL_DESC dd = {};
	dd.DepthEnable = true;
	dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&dd, &m_skyDepthState);



	//Setting up blender state

	// Create a rasterizer description and then state
	D3D11_RASTERIZER_DESC blendRasterizerDesc = {};
	blendRasterizerDesc.CullMode = D3D11_CULL_NONE;
	blendRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	device->CreateRasterizerState(&blendRasterizerDesc, &m_blendRasterizer);

	// Depth state off?
	D3D11_DEPTH_STENCIL_DESC blendDepthStencilDesc = {};
	blendDepthStencilDesc.DepthEnable = false;
	device->CreateDepthStencilState(&blendDepthStencilDesc, &m_blendDepthState);
	
	//context->OMSetDepthStencilState(depthState, 0);

	//Setup blendstate for the transperant group
	D3D11_BLEND_DESC bd = {};
	bd.RenderTarget[0].BlendEnable = true;

	// Settings for blending RGB channels
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	// Settings for blending alpha channel
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// Setting for masking out individual color channels
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// Create the state
	device->CreateBlendState(&bd, &m_blendState);

}


void Renderer::InitializeShaders()
{
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	// Add if successful
	if (device->CreateSamplerState(&desc, &sampler) == 0)
		m_sampler = sampler;

	m_vs = new SimpleVertexShader(device, context);
	m_vs->LoadShaderFile(L"VertexShader.cso");

	m_ps = new SimplePixelShader(device, context);
	m_ps->LoadShaderFile(L"PixelShader.cso");

	m_shadowVS = new SimpleVertexShader(device, context);
	m_shadowVS->LoadShaderFile(L"ShadowVS.cso");

	m_skyVS = new SimpleVertexShader(device, context);
	m_skyVS->LoadShaderFile(L"SkyVertexShader.cso");

	m_skyPS = new SimplePixelShader(device, context);
	m_skyPS->LoadShaderFile(L"SkyPixelShader.cso");


	m_blendPS = new SimplePixelShader(device, context);
	m_blendPS->LoadShaderFile(L"BlendingPixelShader.cso");
}

void Renderer::InitializeShadowMaps()
{
	// Create shadow requirements ------------------------------------------
	shadowMapSize = 4096;

	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapSize;
	shadowDesc.Height = shadowMapSize;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);

	// Create the depth/stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &m_shadowDSV);

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture, &srvDesc, &m_shadowSRV);

	// Release the texture reference since we don't need it
	shadowTexture->Release();

	// Create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &m_shadowSampler);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &m_shadowRasterizer);

	XMMATRIX shProj = XMMatrixOrthographicLH(
		20,
		20,
		0.1f,
		50);
	XMStoreFloat4x4(&m_shadowProjectionMatrix, XMMatrixTranspose(shProj));
}

void Renderer::Begin()
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

	// Reset depth stencil state
	context->OMSetDepthStencilState(0, 0);
	context->OMSetBlendState(0, 0, 0xFFFFFF);
}

void Renderer::End()
{
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
}

void Renderer::RenderGroup(DrawGroup& drawGroup)
{
	// Create the view and projection for the shadow map light
	XMMATRIX shView = XMMatrixLookToLH(
		XMVectorAdd(XMLoadFloat3(&drawGroup.m_camera.GetPosition()), XMVectorMultiply(XMLoadFloat3(&drawGroup.m_lightList[0].Direction), XMVectorSet(-D_LIGHT_SHADOW_DISTANCE, -D_LIGHT_SHADOW_DISTANCE, -D_LIGHT_SHADOW_DISTANCE, 0))),
		XMLoadFloat3(&drawGroup.m_lightList[0].Direction),
		XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&m_shadowViewMatrix, XMMatrixTranspose(shView));

	// Set up the initial pipeline state for shadow map creation
	context->OMSetRenderTargets(0, 0, m_shadowDSV);
	context->ClearDepthStencilView(m_shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(m_shadowRasterizer);

	// Update the viewport to describe rendering into
	// the full shadow map resolution
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)shadowMapSize;
	vp.Height = (float)shadowMapSize;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	// Set up shaders for rendering
	m_shadowVS->SetShader();
	m_shadowVS->SetMatrix4x4("view", m_shadowViewMatrix);
	m_shadowVS->SetMatrix4x4("projection", m_shadowProjectionMatrix);

	// Turn OFF the pixel shader
	context->PSSetShader(0, 0, 0);

	// Render to the shadow map
	for (size_t i = 0; i < drawGroup.m_visibleCount; i++)
	{
		Mesh* mesh = AssetManager::get().GetMeshPointer(drawGroup.m_opaqueObjects[i].GetMeshHandle());
		RenderToShadowMap(drawGroup.m_opaqueObjects[i].GetTransform(), mesh);
	}
	//Resetting the render states
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	vp.Width = (float)width;
	vp.Height = (float)height;
	context->RSSetViewports(1, &vp);
	context->RSSetState(0);

	// Render the scene
	for (size_t i = 0; i < drawGroup.m_visibleCount; i++)
	{
		RenderVisibleEntity(drawGroup.m_opaqueObjects[i], drawGroup.m_camera, drawGroup.m_lightList, drawGroup.m_lightCount);
	}
	// Draw the sky AFTER all opaque geometry
	DrawSky(drawGroup.m_camera);
	
	// Set the states!
	context->RSSetState(m_blendRasterizer);
	context->OMSetBlendState(m_blendState, 0, 0xFFFFFFFF);
	for (size_t i = 0; i < drawGroup.m_transparentCount; i++)
	{
		//Render transperant obj
		RenderTransperentEntity(drawGroup.m_transparentObjects[i].m_entity, drawGroup.m_camera, drawGroup.m_lightList, drawGroup.m_lightCount, drawGroup.m_transparentObjects[i].m_transparency);
	}
	
	// Resetting blender state
	context->OMSetBlendState(0, 0, 0xFFFFFFFF);
	context->RSSetState(0);
	// Turn off all texture at the pixel shader stage
	// This is to ensure that when we draw to shadowSRV next time, it is not bound to anything.
	ID3D11ShaderResourceView* noSRV[16] = {};
	context->PSSetShaderResources(0, 16, noSRV);
}

void Renderer::Render(SimplePixelShader* ps, SimpleVertexShader* vs, Material* mat, ID3D11SamplerState* sampler, DirectX::XMFLOAT4X4& transform, Mesh* mesh, Camera& camera, Light* lights, int lightCount, float transparency)
{
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	vs->SetMatrix4x4("world", transform);
	vs->SetMatrix4x4("view", camera.GetView());
	vs->SetMatrix4x4("projection", camera.GetProjection());
	vs->SetMatrix4x4("shadowView", m_shadowViewMatrix);
	vs->SetMatrix4x4("shadowProj", m_shadowProjectionMatrix);

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	vs->CopyAllBufferData();

	ps->SetInt("lightCount", (int)lightCount);
	ps->SetData("lights", (void*)(lights), sizeof(Light) * MAX_LIGHTS);
	if (ps == m_blendPS) {
		ps->SetFloat("transparency", transparency);
	}
	// Only copies first ten as the size is fixed on the shader. Subtracting the pad value is necessary because the 
	ps->SetShaderResourceView("diffuseTexture", *AssetManager::get().GetTexturePointer(mat->GetDiffuseTextureHandle()));
	ps->SetShaderResourceView("roughnessTexture", *AssetManager::get().GetTexturePointer(mat->GetRoughnessTextureHandle()));
	ps->SetShaderResourceView("ShadowMap", m_shadowSRV);
	ps->SetSamplerState("basicSampler", sampler);
	ps->SetSamplerState("ShadowSampler", m_shadowSampler);
	ps->CopyAllBufferData();

	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	vs->SetShader();
	ps->SetShader();

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

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

void Renderer::RenderToShadowMap(DirectX::XMFLOAT4X4& transform, Mesh* mesh)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Grab the data from the first entity's mesh
	ID3D11Buffer* vb = mesh->GetVertexBuffer();
	ID3D11Buffer* ib = mesh->GetIndexBuffer();

	// Set buffers in the input assembler
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	// Finalize vertex shader data
	m_shadowVS->SetMatrix4x4("world", transform);
	m_shadowVS->CopyAllBufferData();

	// Finally do the actual drawing
	context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
}

void Renderer::RenderTransperentEntity(DrawItem& entity, Camera& camera, Light* lights, int lightCount,float transperency)
{
	Material* mat = AssetManager::get().GetMaterialPointer(entity.GetMaterialHandle());
	Mesh* mesh = AssetManager::get().GetMeshPointer(entity.GetMeshHandle());
	
	Render(m_blendPS, m_vs, mat, m_sampler, entity.GetTransform(), mesh, camera, lights, lightCount,transperency);
}


void Renderer::RenderVisibleEntity(DrawItem& entity, Camera& camera, Light* lights, int lightCount)
{
	Material* mat = AssetManager::get().GetMaterialPointer(entity.GetMaterialHandle());
	Mesh* mesh = AssetManager::get().GetMeshPointer(entity.GetMeshHandle());

	Render(m_ps, m_vs, mat, m_sampler, entity.GetTransform(), mesh, camera, lights, lightCount);
}

void Renderer::DrawSky(Camera& camera)
{

	Mesh* mesh = AssetManager::get().GetMeshPointer(1);

	ID3D11Buffer* vb = mesh->GetVertexBuffer();
	ID3D11Buffer* ib = mesh->GetIndexBuffer();
	
	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	// Set up shaders
	m_skyVS->SetMatrix4x4("view", camera.GetView());
	m_skyVS->SetMatrix4x4("projection", camera.GetProjection());
	m_skyVS->CopyAllBufferData();
	m_skyVS->SetShader();

	m_skyPS->SetShaderResourceView("Sky", m_skySRV);
	m_skyPS->SetSamplerState("BasicSampler", m_sampler);
	m_skyPS->SetShader();


	// Set up sky-specific render states
	context->RSSetState(m_skyRasterState);
	context->OMSetDepthStencilState(m_skyDepthState, 0);

	// Draw
	context->DrawIndexed(mesh->GetIndexCount(), 0, 0);

	// Reset your states
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);

}