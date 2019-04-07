#pragma once
#include <string>
#include "Vector2.h"
#include <SpriteBatch.h>
#include <DirectXMath.h>

// Used to store location data on the screen
struct UITransform
{
	// Location of element in parent's space
	Vector2 m_anchor;
	// Size of element relative to parent's size
	Vector2 m_size;
	// Location on image that lines up with the anchor
	Vector2 m_pivot;

	// Parent element/transform
	// Note: Objects cannot be parented to objects added after them
	int m_parent = -1;
};

// Used to store the transform and image of a ui element
struct UIElement
{
	UITransform m_transform;
	int m_textureHandle = -1;
	// TODO: CREATE UI EVENT SYSTEM
	int m_eventBinding = -1;

	// Baked into element by the graph based on screen size
	// Usage outside of ui system not recommended
	RECT m_rect;
	// Color tinting the ui element
	DirectX::XMFLOAT4 m_color = DirectX::XMFLOAT4(1, 1, 1, 1);
};

