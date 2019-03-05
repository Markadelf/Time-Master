#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
	// Little initialization is done here, we assume external initialization for now
	viewDirty = true;
}


Camera::~Camera()
{
}

XMFLOAT4X4 Camera::GetView()
{
	if (viewDirty)
	{
		recalculateViewMatrix();
	}
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjection()
{
	return projectionMatrix;
}

void Camera::SetAspectRatio(float aspectRatio)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		aspectRatio,				// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

XMFLOAT3 Camera::GetPosition()
{
	return position;
}

void Camera::SetPosition(DirectX::XMFLOAT3 pos)
{
	position = pos;
	viewDirty = true;
}

void Camera::Move(DirectX::XMFLOAT3 displacement)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR disp = XMLoadFloat3(&displacement);

	pos = XMVectorAdd(pos, disp);

	XMStoreFloat3(&position, pos);
	viewDirty = true;
}

void Camera::MoveRelative(DirectX::XMFLOAT3 displacement)
{
	if (viewDirty)
	{
		recalculateViewMatrix();
	}

	XMVECTOR pos = XMLoadFloat3(&position);
	// Get forward component
	XMVECTOR disp = XMLoadFloat3(&direction);
	// Get up component
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	// Add right component
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(disp, up)) * displacement.x;
	// Add up component
	disp *= displacement.z;
	disp += right;
	up *= displacement.y;
	disp += up;

	pos = XMVectorAdd(pos, disp);

	XMStoreFloat3(&position, pos);
	viewDirty = true;
}

float Camera::GetYaw()
{
	return yaw;
}

void Camera::SetYaw(float yaw)
{
	yaw = fmodf(yaw, XM_2PI);

	this->yaw = yaw;
	viewDirty = true;
}

float Camera::GetPitch()
{
	return pitch;
}

void Camera::SetPitch(float pitch)
{
	if (pitch < -XM_PIDIV2)
	{
		pitch = -XM_PIDIV2;
	}
	if (pitch > XM_PIDIV2)
	{
		pitch = XM_PIDIV2;
	}
	this->pitch = pitch;
	viewDirty = true;
}

XMFLOAT4 Camera::GetRot()
{
	XMFLOAT4 ret;
	XMStoreFloat4(&ret, XMQuaternionRotationRollPitchYaw(pitch, yaw, 0));
	return ret;
}

void Camera::recalculateViewMatrix()
{
	XMVECTOR dir = XMVector3Rotate(XMVectorSet(0,0,1,0), XMQuaternionRotationRollPitchYaw(pitch, yaw, 0));
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&position), dir, XMVectorSet(0, 1, 0, 0))));

	XMStoreFloat3(&direction, dir);

	viewDirty = false;
}
