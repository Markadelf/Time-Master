#pragma once
#include <DirectXMath.h>

#define MAX_LIGHTS 128

// Light types
// Must match definitions in shader
#define LIGHT_TYPE_DIRECTIONAL  0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2

struct Light
{
	int					Type;
	DirectX::XMFLOAT3	Direction;	// 16 bytes

	float				Range;
	DirectX::XMFLOAT3	Position;	// 32 bytes

	float				SpotFalloff;
	DirectX::XMFLOAT3	Color;		// 48 bytes

	float				AmbientIntensity;
	float				DiffuseIntensity;
	float	        	Padding_1;
	float	        	Padding_2;	// 64 bytes
};

// Not passed to shaders, but is still heavily related to rendering lights
//struct ShadowMapInfo 
//{
//	ID3D11DepthStencilView* shadowDSV;
//	ID3D11ShaderResourceView* shadowSRV;
//	DirectX::XMFLOAT4X4 shadowViewMatrix;
//	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
//};