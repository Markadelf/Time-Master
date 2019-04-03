#pragma once
#include "Transform.h"
#include "Collider.h"
#include "HandleObject.h"

// Static objects are ones that don't move
class StaticObject
{
	Transform m_transform;
	HandleObject m_handle;

public:
	StaticObject();
	StaticObject(Transform& t, HandleObject& obj);
	~StaticObject();
	
	Transform GetTransform() const;
	HandleObject GetHandles() const;

};

