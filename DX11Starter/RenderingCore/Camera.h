#pragma once

#include <DirectXMath.h>
#include "DXCore.h"

class Camera
{
private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT3 direction;

	DirectX::XMFLOAT3 position;

	// rotation components
	float yaw;
	float pitch;

	// true if view matrix is dirty
	bool viewDirty;

public:
	Camera();
	~Camera();

	// Get the camera matrices
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();

	// Set the aspect ratio
	void SetAspectRatio(float);
	
	// Repositioning logic
	DirectX::XMFLOAT3 GetPosition();
	void SetPosition(DirectX::XMFLOAT3);
	void Move(DirectX::XMFLOAT3 displacement);
	void MoveRelative(DirectX::XMFLOAT3 displacement);

	// Get relative position
	DirectX::XMFLOAT3 GetRelative(DirectX::XMFLOAT3 displacement);

	// Modify the yaw
	float GetYaw();
	void SetYaw(float);

	// Modify the pitch
	float GetPitch();
	void SetPitch(float);

	DirectX::XMFLOAT4 GetRot();

private:
	// Internal function for fixing the view matrix
	void recalculateViewMatrix();
};

