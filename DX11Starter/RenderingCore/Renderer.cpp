#include "Renderer.h"
#include "Vertex.h"
#include "FilePathHelper.h"
#include <cmath>
#include "AssetManager.h"

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

	m_sampler->Release();
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
	for (size_t i = 0; i < drawGroup.m_visibleCount; i++)
	{
		RenderVisibleEntity(drawGroup.m_opaqueObjects[i], drawGroup.m_camera, drawGroup.m_lightList, drawGroup.m_lightCount);
	}
}

void Renderer::Render(SimplePixelShader* ps, SimpleVertexShader* vs, Material* mat, ID3D11SamplerState* sampler, DirectX::XMFLOAT4X4& transform, Mesh* mesh, Camera& camera, Light* lights, int lightCount)
{
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	vs->SetMatrix4x4("world", transform);
	vs->SetMatrix4x4("view", camera.GetView());
	vs->SetMatrix4x4("projection", camera.GetProjection());

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	vs->CopyAllBufferData();

	ps->SetInt("lightCount", (int)lightCount);
	ps->SetData("lights", (void*)(lights), sizeof(Light) * MAX_LIGHTS);
	// Only copies first ten as the size is fixed on the shader. Subtracting the pad value is necessary because the 
	ps->SetShaderResourceView("diffuseTexture", *AssetManager::get().GetTexturePointer(mat->GetDiffuseTextureHandle()));
	ps->SetShaderResourceView("roughnessTexture", *AssetManager::get().GetTexturePointer(mat->GetRoughnessTextureHandle()));
	ps->SetSamplerState("basicSampler", sampler);
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

void Renderer::RenderVisibleEntity(DrawItem& entity, Camera& camera, Light* lights, int lightCount)
{
	Material* mat = AssetManager::get().GetMaterialPointer(entity.GetMaterialHandle());
	Mesh* mesh = AssetManager::get().GetMeshPointer(entity.GetMeshHandle());
	
	Render(m_ps, m_vs, mat, m_sampler, entity.GetTransform(), mesh, camera, lights, lightCount);
}
