#pragma once
#include "TimeInstableTransform.h"
#include "Phenomena.h"
#include <Serializer.h>

// This is how the client will send keyframe data to the server
struct KeyFrameData
{
	Transform m_transform;
	TimeStamp m_timeStamp;
	int m_entityId;
	bool m_usedAction;

	KeyFrameData();
	KeyFrameData(Transform transform, TimeStamp time, int id, bool shot);
	~KeyFrameData();

	// Serialization
	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

