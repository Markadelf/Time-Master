#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"
#include "StaticObject.h"
#include "PlayerKeyFrameData.h"
#include "GameInput.h"

class Player
{
	// Most Recent Transform of the Player
	Transform m_transform;

	HandleObject m_handle;

	// Other Variables
	bool m_dead;
	bool m_reversed;				// false if moving forward in time
	bool m_shot;
	int m_entityId;
	TimeStamp m_time;

	TimeStamp m_lastTimeShot;

public:
	Player();
	~Player();

	void Initialize(const Transform& startingPos, float initialTime, HandleObject handle);
	void Update(float deltaTime);

	// Accessor functions
	Transform GetTransform() const;
	TimeStamp GetTimeStamp() const;
	TimeStamp GetTimeShot() const;
	bool GetReversed() const;
	int GetEntityId() const;

	HandleObject GetHandle() const;

	// Modifier functions
	void SetHandle(HandleObject& obj);
	void Rotate(float amount);
	void SetEntityId(int id);
	void SetTransform(Transform trans);

	// Getting the keyframe, modifies the last time shot property
	KeyFrameData GetKeyFrame();

	// Player Input
	GameInput *PlayerInput;
	void acquireInput(float deltaTime);

private:
	void UpdatePosition(float deltaTime);
};

