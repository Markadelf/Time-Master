#include "Projectile.h"

Projectile::Projectile(): Projectile(TimeInstableTransform(), HandleObject())
{
}

Projectile::Projectile(TimeInstableTransform trajectory, HandleObject handles)
{
	m_transform = trajectory;
	m_handles = handles;
}

Projectile::~Projectile()
{
}

TimeInstableTransform Projectile::GetTransform() const
{
	return m_transform;
}

HandleObject Projectile::GetHandles() const
{
	return m_handles;
}
