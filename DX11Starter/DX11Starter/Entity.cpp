#include "Entity.h"

using namespace DirectX;
Entity::Entity()
{
	XMStoreFloat4x4(&transform, XMMatrixIdentity());
	position = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1.f, 1.f, 1.f);
	XMStoreFloat4(&rotation, XMQuaternionIdentity());
	
	// Intiialize with a known illegal mesh handle
	meshHandle = -1;

	// The Matrix is an identity so it isn't dirty
	isDirty = false; 
}

Entity::Entity(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos): Entity()
{
	this->meshHandle = meshHandle;
	this->materialHandle = materialHandle;
	position = pos;
	isDirty = true;
}

Entity::Entity(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rot): Entity(meshHandle, materialHandle, pos)
{
	this->scale = scale;
	rotation = rot;
}


Entity::~Entity()
{
}

DirectX::XMFLOAT4X4 Entity::GetTransform()
{
	if (isDirty)
	{
		RecalculateMatrix();
	}
	return transform;
}

void Entity::SetPosition(DirectX::XMFLOAT3 pos)
{
	position = pos;
	isDirty = true;
}

void Entity::Move(DirectX::XMFLOAT3 displacement)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR disp = XMLoadFloat3(&displacement);

	pos = XMVectorAdd(pos, disp);

	XMStoreFloat3(&position, pos);
	isDirty = true;
}

void Entity::MoveForward(DirectX::XMFLOAT3 displacement)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR disp = XMLoadFloat3(&displacement);
	XMVECTOR rot = XMLoadFloat4(&rotation);
	disp = XMVector3Rotate(disp, rot);

	pos = XMVectorAdd(pos, disp);

	XMStoreFloat3(&position, pos);
	isDirty = true;
}

DirectX::XMFLOAT3 Entity::GetPosition()
{
	return position;
}

void Entity::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;
	isDirty = true;
}

DirectX::XMFLOAT3 Entity::GetScale()
{
	return scale;
}

void Entity::SetRotation(DirectX::XMFLOAT4 rot)
{
	rotation = rot;
	isDirty = true;
}

DirectX::XMFLOAT4 Entity::GetRotation()
{
	return rotation;
}

int Entity::GetMeshHandle()
{
	return meshHandle;
}

int Entity::GetMaterialHandle()
{
	return materialHandle;
}

void Entity::RecalculateMatrix()
{
	// Scale, rotate, then translate
	XMMATRIX matrix = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
	matrix = XMMatrixMultiply(matrix, XMMatrixRotationQuaternion(XMLoadFloat4(&rotation)));
	matrix = XMMatrixMultiply(matrix, XMMatrixTranslationFromVector(XMLoadFloat3(&position)));

	XMStoreFloat4x4(&transform, XMMatrixTranspose(matrix));

	isDirty = false;
}
