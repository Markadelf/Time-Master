#pragma once
#include <DirectXMath.h>

// Currently unused
// Will be useful for rendering objects that don't need to be updated from the scene graph
class DrawItem
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
	DrawItem();
	DrawItem(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos);
	DrawItem(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rot);
	DrawItem(int meshHandle, int materialHandle, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rot);
	~DrawItem();

	// Accessors
	DirectX::XMFLOAT4X4 GetTransform();

	void SetPosition(DirectX::XMFLOAT3 pos);
	void Move(DirectX::XMFLOAT3 displacement);
	void MoveForward(DirectX::XMFLOAT3 displacement);
	DirectX::XMFLOAT3 GetPosition();

	void SetScale(DirectX::XMFLOAT3 scale);
	DirectX::XMFLOAT3 GetScale();

	void SetRotation(DirectX::XMFLOAT3 rot);
	void SetRotation(DirectX::XMFLOAT4 rot);
	DirectX::XMFLOAT4 GetRotation();

	void SetMeshHandle(int val);
	int GetMeshHandle();
	void SetMaterialHandle(int val);
	int GetMaterialHandle();

private:
	// Fixes matrix
	void RecalculateMatrix();

};

