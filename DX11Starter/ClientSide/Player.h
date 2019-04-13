#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"
#include "StaticObject.h"

class Player
{
	// Most Recent Transform of the Player
	Transform m_transform;

	HandleObject m_handle;

	// Other Variables
	bool m_dead;
	bool m_reversed;				// false if moving forward in time
	int m_entityId;
	TimeStamp m_time;

	TimeStamp m_lastTimeShot;

public:
	Player();
	~Player();

	void Initialize(const Transform& startingPos, float initialTime, HandleObject handle);
	void Update(float deltaTime, StaticObject* statics, int staticCount);

	// Accessor functions
	Transform GetTransform() const;
	TimeStamp GetTimeStamp() const;
	bool GetReversed() const;
	int GetEntityId() const;

	HandleObject GetHandle() const;

	// Modifier functions
	void SetHandle(HandleObject& obj);
	void Rotate(float amount);
	void SetEntityId(int id);

private:
	void UpdatePosition(float deltaTime, StaticObject* statics, int staticCount);
};

