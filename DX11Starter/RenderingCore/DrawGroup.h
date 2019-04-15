#pragma once
#include "Camera.h"
#include "DrawItem.h"

struct TransparentEntity {
	DrawItem m_entity;
	float m_transparency;
};

// Used to communicate Draw Info to the Renderer
struct DrawGroup
{
	// Camera used to render this group to the screen
	Camera m_camera;

	// List of fully opaque objects
	DrawItem m_opaqueObjects[100];
	TransparentEntity m_transparentObjects[100];

	int m_visibleCount = 0;
	int m_transparentCount = 0;

	inline void PushVisible(DrawItem& item) {
		m_opaqueObjects[m_visibleCount++] = item;
	}
};

