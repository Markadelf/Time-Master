#include "Phenomina.h"

Phenomina::Phenomina(): Phenomina(TimeInstableTransform(), HandleObject())
{
}

Phenomina::Phenomina(TimeInstableTransform trajectory, HandleObject handles)
{
	m_transform = trajectory;
	m_handles = handles;
}

Phenomina::~Phenomina()
{
}

TimeInstableTransform Phenomina::GetTransform() const
{
	return m_transform;
}

HandleObject Phenomina::GetHandle() const
{
	return m_handles;
}

bool Phenomina::Serialize(Buffer& buffer) const
{
	return m_transform.Serialize(buffer) && m_handles.Serialize(buffer);
}

bool Phenomina::Deserialize(Buffer& buffer)
{
	return m_transform.Deserialize(buffer) && m_handles.Deserialize(buffer);
}
