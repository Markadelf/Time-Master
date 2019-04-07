#pragma once
#include "TimeInstableTransform.h"
#include "KeyFrameData.h"

// This is how the server will send keyframe data to the client
class Phantom
{
	TimeInstableTransform m_transform;
	int m_entityId;
	TimeStamp m_shotTime;	// -1 if no shot was fired
	bool m_shot;
	bool m_activeHead;		// True if this phantom reflects a live character

public:
	Phantom();
	Phantom(TimeInstableTransform transform, int id, bool activeHead, bool shot, TimeStamp timeShot = -1);
	Phantom(TimeInstableTransform transform, KeyFrameData keyFrame);
	~Phantom();

	// Accessor
	TimeInstableTransform GetTransform();
	int GetPlayerId();
	float GetShotTime();
	bool IsActiveHead();
	bool GetShot();

	// Modifier
	void Trim(TimeStamp time);

	// Serialization
	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

