#include "Emitter.h"

using namespace DirectX;

float globalDeltaTime = 0.0f;
Emitter::Emitter() {

}

Emitter::Emitter(
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
	int texHandle) : Emitter()
{
	// Save params
//	this->vs = vs;
	//this->ps = ps;
	this->texture = texHandle;

	

	this->maxParticles = maxParticles;
	this->lifetime = lifetime;
	this->startColor = startColor;
	this->endColor = endColor;
	this->startVelocity = startVelocity;
	this->startSize = startSize;
	this->endSize = endSize;
	this->particlesPerSecond = particlesPerSecond;
	this->secondsPerParticle = 1.0f / particlesPerSecond;

	this->velocityRandomRange = velocityRandomRange;
	this->positionRandomRange = positionRandomRange;
	this->rotationRandomRanges = rotationRandomRanges;

	this->emitterPosition = emitterPosition;
	this->emitterAcceleration = emitterAcceleration;

	timeSinceEmit = 0;
	livingParticleCount = 0;
	firstAliveIndex = 0;
	firstDeadIndex = 0;

	// Make the particle array
	particles = new Particle[maxParticles];
	ZeroMemory(particles, sizeof(Particle) * maxParticles);

	// Create local particle vertices

	//// Create UV's here, as those will never change
	//localParticleVertices = new ParticleVertex[4 * maxParticles];
	//for (int i = 0; i < maxParticles * 4; i += 4)
	//{
	//	localParticleVertices[i + 0].UV = XMFLOAT2(0, 0);
	//	localParticleVertices[i + 1].UV = XMFLOAT2(1, 0);
	//	localParticleVertices[i + 2].UV = XMFLOAT2(1, 1);
	//	localParticleVertices[i + 3].UV = XMFLOAT2(0, 1);
	//}


	// Create buffers for drawing particles

	//// DYNAMIC vertex buffer (no initial data necessary)
	//D3D11_BUFFER_DESC vbDesc = {};
	//vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	//vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;
	//device->CreateBuffer(&vbDesc, 0, &vertexBuffer);

	// Index buffer data
	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	// Regular (static) index buffer
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
	device->CreateBuffer(&ibDesc, &indexData, &indexBuffer);

	// Just make a single buffer to hold copy of all particle data
	D3D11_BUFFER_DESC allParticleBufferDesc = {};
	allParticleBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	allParticleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	allParticleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	allParticleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	allParticleBufferDesc.StructureByteStride = sizeof(Particle);
	allParticleBufferDesc.ByteWidth = sizeof(Particle) * maxParticles;
	device->CreateBuffer(&allParticleBufferDesc, 0, &particleDataBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxParticles;
	device->CreateShaderResourceView(particleDataBuffer, &srvDesc, &particleDataSRV);

	delete[] indices;
	float timestamp = 0;
	for (int i = 0; i < maxParticles; i++)
	{
		SpawnParticle(timestamp);

		timestamp += secondsPerParticle/2;
	}
}


Emitter::~Emitter()
{
	delete[] particles;
	//delete[] localParticleVertices;
	//vertexBuffer->Release();
	indexBuffer->Release();

	particleDataBuffer->Release();
	particleDataSRV->Release();
}

void Emitter::Update(float dt,float currentTime)
{
	// Update all particles - Check cyclic buffer first
	if (firstAliveIndex < firstDeadIndex)
	{
		// First alive is BEFORE first dead, so the "living" particles are contiguous
		// 
		// 0 -------- FIRST ALIVE ----------- FIRST DEAD -------- MAX
		// |    dead    |            alive       |         dead    |

		// First alive is before first dead, so no wrapping
		for (int i = firstAliveIndex; i < firstDeadIndex; i++)
			UpdateSingleParticle(currentTime, i);
	}
	else
	{
		// First alive is AFTER first dead, so the "living" particles wrap around
		// 
		// 0 -------- FIRST DEAD ----------- FIRST ALIVE -------- MAX
		// |    alive    |            dead       |         alive   |

		// Update first half (from firstAlive to max particles)
		for (int i = firstAliveIndex; i < maxParticles; i++)
			UpdateSingleParticle(currentTime, i);

		// Update second half (from 0 to first dead)
		for (int i = 0; i < firstDeadIndex; i++)
			UpdateSingleParticle(currentTime, i);
	}

	// Add to the time
	timeSinceEmit += dt;

	// Enough time to emit?
	while (timeSinceEmit > secondsPerParticle)
	{
		SpawnParticle(currentTime);
		timeSinceEmit -= secondsPerParticle;
	}
}

void Emitter::UpdateSingleParticle(float dt, int index)
{
	float age = dt - particles[index].SpawnTime;

	// Update and check for death
	if (age >= lifetime)
	{
		// Recent death, so retire by moving alive count
		firstAliveIndex++;
		firstAliveIndex %= maxParticles;
		livingParticleCount--;
		return;
	}
}

void Emitter::SpawnParticle(float currentTime)
{
	// Any left to spawn?
	if (livingParticleCount == maxParticles)
		return;

	// Reset the first dead particle
	particles[firstDeadIndex].SpawnTime = currentTime;

	particles[firstDeadIndex].StartPosition = emitterPosition;
	particles[firstDeadIndex].StartPosition.x += (((float)rand() / RAND_MAX) * 2 - 1) * positionRandomRange.x;
	particles[firstDeadIndex].StartPosition.y += (((float)rand() / RAND_MAX) * 2 - 1) * positionRandomRange.y;
	particles[firstDeadIndex].StartPosition.z += (((float)rand() / RAND_MAX) * 2 - 1) * positionRandomRange.z;
	//particles[firstDeadIndex].StartPosition.x += livingParticleCount/100.0f * positionRandomRange.x;
	//particles[firstDeadIndex].StartPosition.y += livingParticleCount / 100.0f * positionRandomRange.y;
	//particles[firstDeadIndex].StartPosition.z += livingParticleCount / 100.0f * positionRandomRange.z;

	

	particles[firstDeadIndex].StartVelocity = startVelocity;
	particles[firstDeadIndex].StartVelocity.x += (((float)rand() / RAND_MAX) * 2 - 1) * velocityRandomRange.x;
	particles[firstDeadIndex].StartVelocity.y += (((float)rand() / RAND_MAX) * 2 - 1) * velocityRandomRange.y;
	particles[firstDeadIndex].StartVelocity.z += (((float)rand() / RAND_MAX) * 2 - 1) * velocityRandomRange.z;

	float rotStartMin = rotationRandomRanges.x;
	float rotStartMax = rotationRandomRanges.y;
	particles[firstDeadIndex].RotationStart = ((float)rand() / RAND_MAX) * (rotStartMax - rotStartMin) + rotStartMin;

	float rotEndMin = rotationRandomRanges.z;
	float rotEndMax = rotationRandomRanges.w;
	particles[firstDeadIndex].RotationEnd = ((float)rand() / RAND_MAX) * (rotEndMax - rotEndMin) + rotEndMin;

	//// Increment and wrap
	firstDeadIndex++;
	firstDeadIndex %= maxParticles;

	livingParticleCount++;
}

//void Emitter::CopyParticlesToGPU(ID3D11DeviceContext* context, Camera* camera)
//{
//	// Update local buffer (living particles only as a speed up)
//
//	// Check cyclic buffer status
//	if (firstAliveIndex < firstDeadIndex)
//	{
//		for (int i = firstAliveIndex; i < firstDeadIndex; i++)
//			CopyOneParticle(i, camera);
//	}
//	else
//	{
//		// Update first half (from firstAlive to max particles)
//		for (int i = firstAliveIndex; i < maxParticles; i++)
//			CopyOneParticle(i, camera);
//
//		// Update second half (from 0 to first dead)
//		for (int i = 0; i < firstDeadIndex; i++)
//			CopyOneParticle(i, camera);
//	}
//
//	// All particles copied locally - send whole buffer to GPU
//	D3D11_MAPPED_SUBRESOURCE mapped = {};
//	context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
//
//	memcpy(mapped.pData, localParticleVertices, sizeof(ParticleVertex) * 4 * maxParticles);
//
//	context->Unmap(vertexBuffer, 0);
//}

//void Emitter::CopyOneParticle(int index, Camera* camera)
//{
//	int i = index * 4;
//
//	localParticleVertices[i + 0].Position = CalcParticleVertexPosition(index, 0, camera);
//	localParticleVertices[i + 1].Position = CalcParticleVertexPosition(index, 1, camera);
//	localParticleVertices[i + 2].Position = CalcParticleVertexPosition(index, 2, camera);
//	localParticleVertices[i + 3].Position = CalcParticleVertexPosition(index, 3, camera);
//
//	localParticleVertices[i + 0].Color = particles[index].Color;
//	localParticleVertices[i + 1].Color = particles[index].Color;
//	localParticleVertices[i + 2].Color = particles[index].Color;
//	localParticleVertices[i + 3].Color = particles[index].Color;
//}


//XMFLOAT3 Emitter::CalcParticleVertexPosition(int particleIndex, int quadCornerIndex, Camera* camera)
//{
//	// Get the right and up vectors out of the view matrix
//	// (Remember that it is probably already transposed)
//	XMFLOAT4X4 view = camera->GetView();
//	XMVECTOR camRight = XMVectorSet(view._11, view._12, view._13, 0);
//	XMVECTOR camUp = XMVectorSet(view._21, view._22, view._23, 0);
//
//	// Determine the offset of this corner of the quad
//	// Since the UV's are already set when the emitter is created, 
//	// we can alter that data to determine the general offset of this corner
//	XMFLOAT2 offset = localParticleVertices[particleIndex * 4 + quadCornerIndex].UV;
//	offset.x = offset.x * 2 - 1;	// Convert from [0,1] to [-1,1]
//	offset.y = (offset.y * -2 + 1);	// Same, but flip the Y
//
//	// Load into a vector, which we'll assume is float3 with a Z of 0
//	// Create a Z rotation matrix and apply it to the offset
//	XMVECTOR offsetVec = XMLoadFloat2(&offset);
//	XMMATRIX rotMatrix = XMMatrixRotationZ(particles[particleIndex].Rotation);
//	offsetVec = XMVector3Transform(offsetVec, rotMatrix);
//
//	// Add and scale the camera up/right vectors to the position as necessary
//	XMVECTOR posVec = XMLoadFloat3(&particles[particleIndex].Position);
//	posVec += camRight * XMVectorGetX(offsetVec) * particles[particleIndex].Size;
//	posVec += camUp * XMVectorGetY(offsetVec) * particles[particleIndex].Size;
//
//	// This position is all set
//	XMFLOAT3 pos;
//	XMStoreFloat3(&pos, posVec);
//	return pos;
//}


//
//void ParticleSystem::Draw(ID3D11DeviceContext* context, Camera* camera)
//{
//	// Copy to dynamic buffer
//	CopyParticlesToGPU(context, camera);
//
//	// Set up buffers
//	UINT stride = sizeof(ParticleVertex);
//	UINT offset = 0;
//	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
//	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	//vs->SetMatrix4x4("view", camera->GetView());
//	//vs->SetMatrix4x4("projection", camera->GetProjection());
//	//vs->SetShader();
//	//vs->CopyAllBufferData();
//
//	//ps->SetShaderResourceView("particle", texture);
//	//ps->SetShader();
//
//	// Draw the correct parts of the buffer
//	if (firstAliveIndex < firstDeadIndex)
//	{
//		context->DrawIndexed(livingParticleCount * 6, firstAliveIndex * 6, 0);
//	}
//	else
//	{
//		// Draw first half (0 -> dead)
//		context->DrawIndexed(max(firstDeadIndex - 1, 0) * 6, 0, 0);
//
//		// Draw second half (alive -> max)
//		context->DrawIndexed((maxParticles - firstAliveIndex) * 6, firstAliveIndex * 6, 0);
//	}
//
//}
