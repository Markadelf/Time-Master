#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"

// This class is for storing data about a projectile, its attributes and 
class Projectile
{
	TimeInstableTransform m_transform;
	HandleObject m_handles;

public:
	Projectile();
	Projectile(TimeInstableTransform trajectory, HandleObject handles);
	~Projectile();

	// Accessors
	TimeInstableTransform GetTransform() const;
	HandleObject GetHandles() const;
};
