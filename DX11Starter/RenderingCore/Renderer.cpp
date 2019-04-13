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
	delete particleVS;
	delete particlePS;

	particleBlendState->Release();
	particleDepthState->Release();
	particleDebugRasterState->Release();

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
	InitializeCamera();
	InitializeShaders();
	// A depth state for the particles
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);


	// Blend for particles (additive)
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // Still respect pixel shader output alpha
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &particleBlendState);

	// Debug rasterizer state for particles
	D3D11_RASTERIZER_DESC rd = {};
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = true;
	rd.FillMode = D3D11_FILL_WIREFRAME;
	device->CreateRasterizerState(&rd, &particleDebugRasterState);
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Renderer::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	m_currentView.SetAspectRatio((float)width / height);
}

// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Renderer::InitializeCamera()
{
	m_currentView.SetPosition(XMFLOAT3(0, 0, -3));
	m_currentView.SetYaw(0);
	m_currentView.SetAspectRatio((float)width / height);
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

	particleVS = new SimpleVertexShader(device, context);
	particleVS->LoadShaderFile(L"ParticleVS.cso");

	particlePS = new SimplePixelShader(device, context);
	particlePS->LoadShaderFile(L"ParticlePS.cso");
}

Camera* Renderer::GetCamera()
{
	return &m_currentView;
}

std::vector<DirectionalLight>* Renderer::GetLights()
{
	return &m_lightList;
}

void Renderer::Begin()
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	// Particle states
	float blend[4] = { 1,1,1,1 };
	// Reset to default states for next frame
	context->OMSetBlendState(0, blend, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);
	context->RSSetState(0);
	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
}

void Renderer::End()
{
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
}

void Renderer::DrawScene(ServerSceneGraph* scenegraph, float time)
{
	StaticObject* objs;
	int sCount;
	scenegraph->GetStatics(&objs, sCount);
	for (size_t i = 0; i < sCount; i++)
	{
		RenderObjectAtPos(objs[i].GetHandles(), objs[i].GetTransform());
	}
	int eCount = scenegraph->GetEntityCount();
	for (size_t i = 0; i < eCount; i++)
	{
		RenderPhantoms(*scenegraph->GetEntity((int)i), time);
	}
}

void Renderer::Render(SimplePixelShader* ps, SimpleVertexShader* vs, ID3D11ShaderResourceView* texture, ID3D11SamplerState* sampler, DirectX::XMFLOAT4X4& transform, Mesh* mesh)
{
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	vs->SetMatrix4x4("world", transform);
	vs->SetMatrix4x4("view", m_currentView.GetView());
	vs->SetMatrix4x4("projection", m_currentView.GetProjection());

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	vs->CopyAllBufferData();

	ps->SetInt("lightAmount", (int)m_lightList.size());
	// Only copies first ten as the size is fixed on the shader. Subtracting the pad value is necessary because the 
	ps->SetData("light", (&m_lightList[0]), sizeof(DirectionalLight) * 10 - DirectionalLight::PAD);
	ps->SetShaderResourceView("diffuseTexture", texture);
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

void Renderer::Render(Material* mat, XMFLOAT4X4& transform, int meshHandle)
{
	Render(m_ps, m_vs, *AssetManager::get().GetTexturePointer(mat->GetTextureHandle()), m_sampler, transform, AssetManager::get().GetMeshPointer(meshHandle));
}

void Renderer::RenderEntity(Entity& entity)
{
	Render(AssetManager::get().GetMaterialPointer(entity.GetMaterialHandle()), entity.GetTransform(), entity.GetMeshHandle());
	//Render(materialManager.GetResourcePointer(entity.GetMaterialHandle()), entity.GetTransform(), entity.GetMeshHandle());
}

void Renderer::RenderObjectAtPos(HandleObject& handle, Transform trans)
{
	XMMATRIX matrix = XMMatrixScaling(handle.m_scale[0], handle.m_scale[1], handle.m_scale[2]);
	matrix = XMMatrixMultiply(matrix, XMMatrixRotationRollPitchYaw(0, trans.GetRot(), 0));
	matrix = XMMatrixMultiply(matrix, XMMatrixTranslation(trans.GetPos().GetX(), 0, trans.GetPos().GetY()));
	XMFLOAT4X4 transform;
	XMStoreFloat4x4(&transform, XMMatrixTranspose(matrix));

	Render(AssetManager::get().GetMaterialPointer(handle.m_material), transform, handle.m_mesh);
	//Render(materialManager.GetResourcePointer(handle.m_material), transform, handle.m_mesh);
}

void Renderer::RenderLerpObject(HandleObject& handle, TimeInstableTransform trans, float t)
{
	RenderObjectAtPos(handle, trans.GetTransform(t));
}

void Renderer::RenderPhantoms(TemporalEntity& phantom, float t)
{
	int phantoms = phantom.GetImageCount();
	Phantom* phantomBuffer = phantom.GetPhantomBuffer();
	HandleObject handle = phantom.GetHandle();
	for (size_t i = 0; i < phantoms; i++)
	{
		TimeInstableTransform trans = phantomBuffer[i].GetTransform();
		if (trans.GetEndTime() > t && trans.GetStartTime() <= t)
		{
			RenderLerpObject(handle, trans, t);
		}
	}

	int phenomina = phantom.GetPhenominaCount();
	Phenomina* phenominaBuffer = phantom.GetPhenominaBuffer();
	for (size_t i = 0; i < phenomina; i++)
	{
		TimeInstableTransform trans = phenominaBuffer[i].GetTransform();
		if (trans.GetEndTime() > t && trans.GetStartTime() <= t)
		{
			handle = phenominaBuffer[i].GetHandle();
			RenderLerpObject(handle, trans, t);
		}
	}
}

void Renderer::RenderEmitterSystem(Emitter * emitter)
{

	// Particle states
	float blend[4] = { 1,1,1,1 };
	context->OMSetBlendState(particleBlendState, blend, 0xffffffff);	// Additive blending
	context->OMSetDepthStencilState(particleDepthState, 0);				// No depth WRITING

	// No wireframe debug
	particlePS->SetInt("debugWireframe", 0);
	particlePS->CopyAllBufferData();

	// Draw the emitters
	//emitter->Draw(context, camera);


	// Copy to dynamic buffer
	emitter->CopyParticlesToGPU(context, &m_currentView);

	// Set up buffers
	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &emitter->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(emitter->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	particleVS->SetMatrix4x4("view", m_currentView.GetView());
	particleVS->SetMatrix4x4("projection", m_currentView.GetProjection());
	particleVS->SetShader();
	particleVS->CopyAllBufferData();

	particlePS->SetShaderResourceView("particle", *AssetManager::get().GetTexturePointer("Textures/particle.jpg"));
	particlePS->SetShader();

	// Draw the correct parts of the buffer
	if (emitter->firstAliveIndex < emitter->firstDeadIndex)
	{
		context->DrawIndexed(emitter->livingParticleCount * 6, emitter->firstAliveIndex * 6, 0);
	}
	else
	{
		// Draw first half (0 -> dead)
		context->DrawIndexed(max(emitter->firstDeadIndex - 1, 0) * 6, 0, 0);

		// Draw second half (alive -> max)
		context->DrawIndexed((emitter->maxParticles - emitter->firstAliveIndex) * 6, emitter->firstAliveIndex * 6, 0);
	}

	//if (GetAsyncKeyState('C'))
	//{
	//	context->RSSetState(particleDebugRasterState);
	//	particlePS->SetInt("debugWireframe", 1);
	//	particlePS->CopyAllBufferData();

	//	//emitter->Draw(context, camera);
	//	//	emitter2->Draw(context, camera);
	//		//emitter3->Draw(context, camera);
	//}
}
