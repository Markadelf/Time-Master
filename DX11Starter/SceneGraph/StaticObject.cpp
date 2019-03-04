#include "stdafx.h"
#include "StaticObject.h"


StaticObject::StaticObject()
{
}


StaticObject::~StaticObject()
{
}

Transform StaticObject::GetTransform() const
{
	return m_transform;
}

HandleObject StaticObject::GetHandles() const
{
	return m_handle;
}
