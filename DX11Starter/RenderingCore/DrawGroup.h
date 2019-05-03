#pragma once
#include "Camera.h"
#include "Lights.h"
#include "DrawItem.h"
#include "Emitter.h"

#define MAX_OBJS 1000

struct TransparentEntity {
	DrawItem m_entity;
	float m_transparency;
	float m_distance;
};

struct EmitterDrawInfo {
    DirectX::XMFLOAT3 pos;
    int m_handle;
    float startTime;
    float endTime;
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

    EmitterDrawInfo m_emitters[100];

	Emitter* emitterOne=NULL;

	Emitter* emitterTwo = NULL;

	float time;

	int m_visibleCount = 0;
	int m_transparentCount = 0;
	int m_lightCount = 0;
    int m_emitterCount = 0;

	inline void PushVisible(DrawItem& item) {
		m_opaqueObjects[m_visibleCount++] = item;
	}
};

