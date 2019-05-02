#pragma once
#include "Camera.h"
#include "Lights.h"
#include "DrawItem.h"

#define MAX_OBJS 1000

struct TransparentEntity {
	DrawItem m_entity;
	float m_transparency;
	float m_distance;
};

// Used to communicate Draw Info to the Renderer
struct DrawGroup
{
	// Camera used to render this group to the screen
	Camera m_camera;

	// Light list for rendering this group 
	Light m_lightList[MAX_LIGHTS];

	// List of fully opaque objects
	DrawItem m_opaqueObjects[MAX_OBJS];
	TransparentEntity m_transparentObjects[MAX_OBJS];

	int m_visibleCount = 0;
	int m_transparentCount = 0;
	int m_lightCount = 0;

	inline void PushVisible(DrawItem& item) {
		m_opaqueObjects[m_visibleCount++] = item;
	}
};

