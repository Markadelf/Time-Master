#pragma once
#include "Camera.h"
#include "Lights.h"
#include "DrawItem.h"
#include "Emitter.h"

struct TransparentEntity {
	DrawItem m_entity;
	float m_transparency;
};


// Used to communicate Draw Info to the Renderer
struct DrawGroup
{
	// Camera used to render this group to the screen
	Camera m_camera;

	// Light list for rendering this group 
	Light m_lightList[100];

	// List of fully opaque objects
	DrawItem m_opaqueObjects[100];
	TransparentEntity m_transparentObjects[100];

	Emitter* emitter=NULL;

	float time;

	int m_visibleCount = 0;
	int m_transparentCount = 0;
	int m_lightCount = 0;

	inline void PushVisible(DrawItem& item) {
		m_opaqueObjects[m_visibleCount++] = item;
	}
};

