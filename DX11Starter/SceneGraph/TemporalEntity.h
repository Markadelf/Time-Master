#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"
#include "Phantom.h"
#include "KeyFrameData.h"
#include "ColliderManager.h"

// The player object keeps a record of all of the actions a player has taken
class TemporalEntity
{
	// Stack of positions at various times
	Phantom* m_images;
    // Stack of times in personal time when time was reversed
    float* m_timesReversed;

	// Buffer containing indexes of projectiles
	int* m_phenomenaImages;
	// Buffer containing the Phenomena
	Phenomenon* m_phenomenaBuffer;

	// Most Recent Transform of the Player
	Transform m_currentTransform;

	// Handle object that includes info for rendering and collisions
	HandleObject m_handle;

    // Action that this entity can use
    // TODO: support more than one type of action
    ActionInfo m_action;

	// Buffer Sizes
	int m_maxImages;
	int m_maxPhenomena;
    int m_maxReverses;

	// Current Head of the time stack
	int m_imageCount;
	int m_phenomenaCount;
    int m_reverseCount;

	// Other Variables
	PhenomenaHandle m_killedBy;		// -1 if alive
	bool m_reversed;				// false if moving forward in time
	int m_entityId;
	float m_lastTimeStamp;

public:
	TemporalEntity();
	TemporalEntity(int maxImages, int maxPhenomena, const Transform& startingPos, float initialTime, HandleObject handle, int entityId);
	~TemporalEntity();

	// Initialize
	void Initialize(const Transform& startingPos, float initialTime, HandleObject handle);
	void Initialize(int maxImages, int maxPhenomena, int entityId);

	// Accessor functions
	PhenomenaHandle GetKilledBy();
	bool GetDead();
	Phantom Head();
	int GetImageCount() const;
	int GetPhenomenaCount() const;
	Phantom* GetPhantomBuffer() const;
	Phenomenon* GetPhenomenaBuffer() const;
	Transform GetTransform() const;
	TimeStamp GetTimeStamp() const;
	bool GetReversed() const;
    ActionInfo GetAction() const;

	HandleObject GetHandle() const;

	// Modifier functions
	void SetHandle(HandleObject& obj);
    void SetAction(ActionInfo& action);

	// Upkeep/Update functions
	
	// Creates a phantom on the player's phantom stack and returns info for that phantom
	Phantom* StackKeyFrame(KeyFrameData keyFrame);
	bool TrackPhantom(Phantom key);
	bool TrackPhenomena(Phenomenon Phenomena);

	// Kill a player at a given time
	void Kill(int imageIndex, TimeStamp time, const PhenomenaHandle& murderHandle, PhenomenaHandle& PhenomenaResetHandle);
	void Revive();
	bool CheckRevive(const PhenomenaHandle& resetHandle);

#ifdef CLIENT
    void GetReverseBuffer(float** buffer, int& count);

#endif // CLIENT
};

