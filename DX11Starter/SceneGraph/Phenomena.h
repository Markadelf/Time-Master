#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"

// This struct represents information that can be used to have an entity perform an action
struct ActionInfo
{
    // Amount of time the action lasts in seconds
    float m_duration = 0;

    // Amount of time after which a phenomena is spawned in seconds
    float m_deploymentTime = -1;

    // Handle to a type of phenomena
    int m_phenomenaType = -1;
};

// This struct defines the info needed to populate a phenomena
struct PhenomenaPrototype
{
    // Rendering and collision info
    HandleObject m_handle;

    // Range of the phenomena
    float m_range;

    // Period of the phenomena
    float m_period;
};

// This struct is used to determine which phenomena is being reffered to
struct PhenomenaHandle 
{
	int m_entity;
	int m_phenomena;
	PhenomenaHandle() 
	{
		m_entity = -1;
		m_phenomena = -1;
	}

	PhenomenaHandle(int entity, int phenomena)
	{
		m_entity = entity;
        m_phenomena = phenomena;
    }
};

// This class is for storing data about an instance of a phenomena
class Phenomenon
{
	TimeInstableTransform m_transform;
	HandleObject m_handles;

public:
	Phenomenon();
	Phenomenon(TimeInstableTransform trajectory, HandleObject handles);
	~Phenomenon();

	// Accessors
	TimeInstableTransform GetTransform() const;
	HandleObject GetHandle() const;
};
