#pragma once
#include "TimeInstableTransform.h"
#include "KeyFrameData.h"

// This is how the server will send keyframe data to the client
class Phantom
{
	TimeInstableTransform m_transform;
	int m_entityId;
	bool m_shot;

public:
	Phantom();
	Phantom(TimeInstableTransform transform, int id, bool shot);
	Phantom(TimeInstableTransform transform, KeyFrameData keyFrame);
	~Phantom();

	// Accessor
	TimeInstableTransform GetTransform();
	int GetEntityId();
	bool GetShot();

	// Modifier
	void Trim(TimeStamp time);

	// Serialization
	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

