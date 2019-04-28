#include "Phenomena.h"

Phenomenon::Phenomenon(): Phenomenon(TimeInstableTransform(), HandleObject())
{
}

Phenomenon::Phenomenon(TimeInstableTransform trajectory, HandleObject handles)
{
	m_transform = trajectory;
	m_handles = handles;
}

Phenomenon::~Phenomenon()
{
}

TimeInstableTransform Phenomenon::GetTransform() const
{
	return m_transform;
}

HandleObject Phenomenon::GetHandle() const
{
	return m_handles;
}
