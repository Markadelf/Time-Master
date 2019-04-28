#include "Phenomena.h"

Phenomena::Phenomena(): Phenomena(TimeInstableTransform(), HandleObject())
{
}

Phenomena::Phenomena(TimeInstableTransform trajectory, HandleObject handles)
{
	m_transform = trajectory;
	m_handles = handles;
}

Phenomena::~Phenomena()
{
}

TimeInstableTransform Phenomena::GetTransform() const
{
	return m_transform;
}

HandleObject Phenomena::GetHandle() const
{
	return m_handles;
}
