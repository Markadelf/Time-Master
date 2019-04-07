#pragma once
#include "TimeInstableTransform.h"
#include "Phenomina.h"
#include <Serializer.h>

// This is how the client will send keyframe data to the server
struct KeyFrameData
{
	Transform m_transform;
	TimeStamp m_timeStamp;
	int m_entityId;
	TimeStamp m_shotTime; // -1 if no shot was fired
	bool m_shot;

	KeyFrameData();
	KeyFrameData(Transform transform, TimeStamp time, int id, bool shot, TimeStamp timeShot = -1);
	~KeyFrameData();

	// Serialization
	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

