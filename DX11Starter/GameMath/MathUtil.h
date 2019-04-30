#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Vector2.h"

namespace mathutil 
{
	float Distance(DirectX::XMFLOAT3 src,DirectX::XMFLOAT3 dest)
	{
		DirectX::XMVECTOR srcVec  = DirectX::XMLoadFloat3(&src);
		DirectX::XMVECTOR destVec = DirectX::XMLoadFloat3(&dest);
		DirectX::XMVECTOR subVec  = DirectX::XMVectorSubtract(srcVec, destVec);
		DirectX::XMVECTOR distVec = DirectX::XMVector3Length(subVec);

		float distance = 0;
		DirectX::XMStoreFloat(&distance, distVec);
		return distance;
	}
}