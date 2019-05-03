#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"
#include "StaticObject.h"
#include "PlayerKeyFrameData.h"
#include "GameInput.h"
#include "AudioEngine.h"

class Player
{
	// Most Recent Transform of the Player
	Transform m_transform;

	HandleObject m_handle;

	// Other Variables
	bool m_dead;
	bool m_reversed;				// false if moving forward in time
	bool m_usedAction;
    bool m_reportActionUsed;

    bool m_keyFrameRequested;

    int m_entityId;
	TimeStamp m_time;
    TimeStamp m_keyPeriod;

    // Timer
    TimeStamp m_keyFrameTimer;

    // Action tracking
	TimeStamp m_actionUsedTime;
    ActionInfo m_action;

	//Player Sound
	CAudioEngine PlayerSound;

public:
	Player();
	~Player();

	void Initialize(const Transform& startingPos, float initialTime, HandleObject handle, float keyPeriod, int isFirstBullet);
	void Update(float deltaTime);

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
	void SetTransform(Transform trans);
    void SetAction(ActionInfo action);

	// Getting the keyframe, modifies the last time shot property
	KeyFrameData GetKeyFrame();

	// Player Input
	GameInput *PlayerInput;
	void acquireAction();
	void acquirePosition(float deltaTime);
    bool StackRequested();
	int m_ctr;
	
private:
	void UpdatePosition(float deltaTime);
};

