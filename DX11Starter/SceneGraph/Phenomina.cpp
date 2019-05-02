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
