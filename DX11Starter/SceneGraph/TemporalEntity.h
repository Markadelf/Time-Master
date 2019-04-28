#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"
#include "Phantom.h"
#include "PlayerKeyFrameData.h"
#include "ColliderManager.h"

// The player object keeps a record of all of the actions a player has taken
class TemporalEntity
{
	// Stack of positions at various times
	Phantom* m_images;
	// Buffer containing indexes of projectiles
	int* m_phenomenaImages;
	// Buffer containing the Phenomena
	Phenomena* m_phenomenaBuffer;

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

	// Current Head of the time stack
	int m_imageCount;
	int m_phenomenaCount;

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
	Phantom Head();
	int GetImageCount() const;
	int GetPhenomenaCount() const;
	Phantom* GetPhantomBuffer() const;
	Phenomena* GetPhenomenaBuffer() const;
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
	void TrackPhenomena(Phenomena Phenomena);

	// Kill a player at a given time
	void Kill(int imageIndex, TimeStamp time, const PhenomenaHandle& murderHandle, PhenomenaHandle& PhenomenaResetHandle);
	void Revive();
	bool CheckRevive(const PhenomenaHandle& resetHandle);
};

