#pragma once
#include "Transform.h"
#include <Serializer.h>

typedef float TimeStamp;

// This is a transform that is defined as a LERP between two points
class TimeInstableTransform
{
	Transform m_start;
	Transform m_end;
	TimeStamp m_startTime;
	TimeStamp m_endTime;
	bool m_reversed;

public:
	TimeInstableTransform();
	TimeInstableTransform(Transform start, Transform end, TimeStamp startTime, TimeStamp endTime, bool reversed);
	~TimeInstableTransform();

	// Get properties
	Transform GetTransform(TimeStamp timeStamp) const;

	Vector2 GetPos(TimeStamp timeStamp) const;
	
	float GetRot(TimeStamp timeStamp) const;
	
	TimeStamp GetStartTime() const;

	TimeStamp GetEndTime() const;

	bool GetReversed() const;
	
	void GetBounds(float* min, float* max) const;

	// Modifiers
	void Trim(TimeStamp time);

	// Serialization
	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

