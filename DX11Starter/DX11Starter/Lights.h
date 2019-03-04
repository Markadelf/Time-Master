#pragma once

#include <DirectXMath.h>

// Directional Light Info container
struct DirectionalLight{
	DirectX::XMFLOAT4 AmbientColor;
	DirectX::XMFLOAT4 DiffuseColor;
	DirectX::XMFLOAT3 Direction;
	INT16 padding;

	static const int PAD = 4;
};