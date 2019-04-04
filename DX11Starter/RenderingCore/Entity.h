#pragma once
#include <DirectXMath.h>

class Entity
{
	// Net transformation matrix
	DirectX::XMFLOAT4X4 transform;

	// Position
	DirectX::XMFLOAT3 position;
	// Scale
	DirectX::XMFLOAT3 scale;
	// Rotation
	DirectX::XMFLOAT4 rotation;

	// Handle for the relevant mesh 
	int meshHandle;

	// Handle for the relevant material
	int materialHandle;

	// Tracks if the object is dirty
	bool isDirty;

public:
	Entity();
	Entity(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos);
	Entity(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rot);
	~Entity();

	// Accessors
	DirectX::XMFLOAT4X4 GetTransform();

	void SetPosition(DirectX::XMFLOAT3 pos);
	void Move(DirectX::XMFLOAT3 displacement);
	void MoveForward(DirectX::XMFLOAT3 displacement);
	DirectX::XMFLOAT3 GetPosition();

	void SetScale(DirectX::XMFLOAT3 scale);
	DirectX::XMFLOAT3 GetScale();

	void SetRotation(DirectX::XMFLOAT4 rot);
	DirectX::XMFLOAT4 GetRotation();

	int GetMeshHandle();
	int GetMaterialHandle();

private:
	// Fixes matrix
	void RecalculateMatrix();

};

