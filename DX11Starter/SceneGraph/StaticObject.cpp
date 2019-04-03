#include "StaticObject.h"


StaticObject::StaticObject()
{
}

StaticObject::StaticObject(Transform& t, HandleObject& obj)
{
	m_transform = t;
	m_handle = obj;
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
