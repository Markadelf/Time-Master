#include "DrawItem.h"

using namespace DirectX;
DrawItem::DrawItem()
{
	position = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1.f, 1.f, 1.f);
	XMStoreFloat4(&rotation, XMQuaternionIdentity());
	
	// Intiialize with a known illegal mesh handle
	meshHandle = -1;

	isDirty = true; 
}

DrawItem::DrawItem(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos): DrawItem()
{
	this->meshHandle = meshHandle;
	this->materialHandle = materialHandle;
	position = pos;
}

DrawItem::DrawItem(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rot): DrawItem(meshHandle, materialHandle, pos)
{
	this->scale = scale;
	rotation = rot;
}

DrawItem::DrawItem(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rot): DrawItem(meshHandle, materialHandle, pos)
{
	this->scale = scale;
	XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z));
}


DrawItem::~DrawItem()
{
}

DirectX::XMFLOAT4X4 DrawItem::GetTransform()
{
	if (isDirty)
	{
		RecalculateMatrix();
	}
	return transform;
}

void DrawItem::SetPosition(DirectX::XMFLOAT3 pos)
{
	position = pos;
	isDirty = true;
}

void DrawItem::Move(DirectX::XMFLOAT3 displacement)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR disp = XMLoadFloat3(&displacement);

	pos = XMVectorAdd(pos, disp);

	XMStoreFloat3(&position, pos);
	isDirty = true;
}

void DrawItem::MoveForward(DirectX::XMFLOAT3 displacement)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR disp = XMLoadFloat3(&displacement);
	XMVECTOR rot = XMLoadFloat4(&rotation);
	disp = XMVector3Rotate(disp, rot);

	pos = XMVectorAdd(pos, disp);

	XMStoreFloat3(&position, pos);
	isDirty = true;
}

DirectX::XMFLOAT3 DrawItem::GetPosition()
{
	return position;
}

void DrawItem::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;
	isDirty = true;
}

DirectX::XMFLOAT3 DrawItem::GetScale()
{
	return scale;
}

void DrawItem::SetRotation(DirectX::XMFLOAT3 rot)
{
	XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z));
}

void DrawItem::SetRotation(DirectX::XMFLOAT4 rot)
{
	rotation = rot;
	isDirty = true;
}

DirectX::XMFLOAT4 DrawItem::GetRotation()
{
	return rotation;
}

void DrawItem::SetMeshHandle(int val)
{
	meshHandle = val;
}

int DrawItem::GetMeshHandle()
{
	return meshHandle;
}

void DrawItem::SetMaterialHandle(int val)
{
	materialHandle = val;
}

int DrawItem::GetMaterialHandle()
{
	return materialHandle;
}

void DrawItem::RecalculateMatrix()
{
	// Scale, rotate, then translate
	XMMATRIX matrix = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
	matrix = XMMatrixMultiply(matrix, XMMatrixRotationQuaternion(XMLoadFloat4(&rotation)));
	matrix = XMMatrixMultiply(matrix, XMMatrixTranslationFromVector(XMLoadFloat3(&position)));

	XMStoreFloat4x4(&transform, XMMatrixTranspose(matrix));

	isDirty = false;
}
