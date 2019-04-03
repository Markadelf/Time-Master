#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"

struct PhenominaHandle 
{
	int m_entity;
	int m_phenomina;
	PhenominaHandle() 
	{
		m_entity = -1;
		m_phenomina = -1;
	}

	PhenominaHandle(int entity, int phenomina)
	{
		m_entity = entity;
		m_phenomina = phenomina;
	}
};

// This class is for storing data about a projectile, its attributes and 
class Phenomina
{
	TimeInstableTransform m_transform;
	HandleObject m_handles;

public:
	Phenomina();
	Phenomina(TimeInstableTransform trajectory, HandleObject handles);
	~Phenomina();

	// Accessors
	TimeInstableTransform GetTransform() const;
	HandleObject GetHandle() const;
};
