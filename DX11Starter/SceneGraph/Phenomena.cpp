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

bool Phenomenon::Serialize(Buffer& buffer) const
{
    return m_transform.Serialize(buffer) && m_handles.Serialize(buffer);
}

bool Phenomenon::Deserialize(Buffer& buffer)
{
    return m_transform.Deserialize(buffer) && m_handles.Deserialize(buffer);
}