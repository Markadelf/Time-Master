#pragma once
// NOT UNNECESSARY
#include "Winsock2.h"
// NOT UNNECESSARY
#include <d3d11.h>
#include <DirectXMath.h>

#include "Camera.h"
//#include "SimpleShader.h"
// For the DirectX Math library
using namespace DirectX;

struct Particle
{
	// 4 floats:
	float SpawnTime;
	DirectX::XMFLOAT3 StartPosition;

	// 4 floats:
	DirectX::XMFLOAT3 StartVelocity;
	float RotationStart;

	// 4 floats:
	float RotationEnd;
	DirectX::XMFLOAT3 padding;
};

//struct ParticleVertex
//{
//	DirectX::XMFLOAT3 Position;
//	DirectX::XMFLOAT2 UV;
//	DirectX::XMFLOAT4 Color;
//};

class Emitter
{
public:
	Emitter(
		int maxParticles,
		int particlesPerSecond,
		float lifetime,
		float startSize,
		float endSize,
		DirectX::XMFLOAT4 startColor,
		DirectX::XMFLOAT4 endColor,
		DirectX::XMFLOAT3 startVelocity,
		DirectX::XMFLOAT3 velocityRandomRange,
		DirectX::XMFLOAT3 emitterPosition,
		DirectX::XMFLOAT3 positionRandomRange,
		DirectX::XMFLOAT4 rotationRandomRanges,
		DirectX::XMFLOAT3 emitterAcceleration,
		ID3D11Device* device,
		int texHandle
		);
	~Emitter();

	void Update(float dt,float currentTime);
//	void Draw(ID3D11DeviceContext* context, Camera* camera);

//private:
	
// Emission properties
	int particlesPerSecond;
	float secondsPerParticle;
	float timeSinceEmit;

	int livingParticleCount;
	float lifetime;

	DirectX::XMFLOAT3 emitterAcceleration;
	DirectX::XMFLOAT3 emitterPosition;
	DirectX::XMFLOAT3 startVelocity;

	DirectX::XMFLOAT3 positionRandomRange;
	DirectX::XMFLOAT3 velocityRandomRange;
	DirectX::XMFLOAT4 rotationRandomRanges; // Min start, max start, min end, max end

	DirectX::XMFLOAT4 startColor; 
	DirectX::XMFLOAT4 endColor;
	float startSize;
	float endSize;

	// Particle array
	Particle* particles;
	int maxParticles;
	int firstDeadIndex;
	int firstAliveIndex;

	// Rendering
	ID3D11Buffer* particleDataBuffer;

//	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11ShaderResourceView* particleDataSRV;
	int texture;
	//ID3D11ShaderResourceView* texture;
	//SimpleVertexShader* vs;
	//SimplePixelShader* ps;

	// Update Methods
	void UpdateSingleParticle(float dt, int index);
	void SpawnParticle(float currentTime);

	// Copy methods
	void CopyParticlesToGPU(ID3D11DeviceContext* context, Camera* camera);
	void CopyOneParticle(int index, Camera* camera);
	DirectX::XMFLOAT3 CalcParticleVertexPosition(int particleIndex, int quadCornerIndex, Camera* camera);
//#include"Renderer.h"
	//friend class Renderer;
};

