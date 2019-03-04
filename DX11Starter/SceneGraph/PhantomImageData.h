#pragma once
#include "TimeInstableTransform.h"
#include "ProjectileManager.h"
#include "Projectile.h"
#include <Serializer.h>

class PhantomImageData
{
	Transform m_transform;
	TimeStamp m_timeStamp;
	int m_playerId;
	TimeStamp m_shotTime; // -1 if no shot was fired
	bool m_shot;

public:
	PhantomImageData();
	PhantomImageData(Transform transform, TimeStamp time, int id, bool shot, TimeStamp timeShot = 0);
	~PhantomImageData();

	// Accessor
	Transform GetTransform();
	TimeStamp GetTimeStamp();
	int GetPlayerId();
	float GetShotTime();
	bool GetShot();

	// Serialization
	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

