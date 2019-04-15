#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"
#include "Phantom.h"
#include "KeyFrameData.h"
#include "ColliderManager.h"

// The player object keeps a record of all of the actions a player has taken
class TemporalEntity
{
	// TODO: REFACTOR PHENOMINA SYSTEM
	// The current system is functional, but I believe it would be better to refactor so that the heavy lifting isn't done by the TemporalEntity class
	// Should be fine for now though
	struct PhenominaCreationInfo
	{
		// Time stamp of shot as it corresponds to the INDEX OF THE IMAGE
		int m_imageIndex;

		// Time Stamp of the shot in GAME TIME
		TimeStamp m_timeStamp;
		PhenominaCreationInfo() 
		{
			m_imageIndex = -1;
			m_timeStamp = 0;
		}

		PhenominaCreationInfo(int image, TimeStamp time) {
			m_imageIndex = image;
			m_timeStamp = time;
		}
	};

	// Stack of positions at various times
	Phantom* m_images;
	// Buffer containing indexes of projectiles
	PhenominaCreationInfo* m_phenominaHandles;
	// Buffer containing the phenomina
	Phenomina* m_phenominaBuffer;

	// Most Recent Transform of the Player
	Transform m_currentTransform;

	// Handle object that includes info for rendering and collisions
	HandleObject m_handle;

	// Buffer Sizes
	int m_maxImages;
	int m_maxPhenomina;

	// Current Head of the time stack
	int m_imageCount;
	int m_phenominaCount;

	// Other Variables
	PhenominaHandle m_killedBy;		// -1 if alive
	bool m_reversed;				// false if moving forward in time
	int m_entityId;
	float m_lastTimeStamp;

public:
	TemporalEntity();
	TemporalEntity(int maxImages, int maxPhenomina, const Transform& startingPos, float initialTime, HandleObject handle, int entityId);
	~TemporalEntity();

	// Initialize
	void Initialize(const Transform& startingPos, float initialTime, HandleObject handle);
	void Initialize(int maxImages, int maxPhenomina, int entityId);

	// Accessor functions
	PhenominaHandle GetKilledBy();
	Phantom Head();
	int GetImageCount() const;
	int GetPhenominaCount() const;
	Phantom* GetPhantomBuffer() const;
	Phenomina* GetPhenominaBuffer() const;
	Transform GetTransform() const;
	TimeStamp GetTimeStamp() const;
	bool GetReversed() const;

	HandleObject GetHandle() const;

	// Modifier functions
	void SetHandle(HandleObject& obj);

	// Upkeep/Update functions
	
	// Creates a phantom on the player's phantom stack and returns info for that phantom
	Phantom* StackKeyFrame(KeyFrameData keyFrame);
	void TrackPhenomina(Phenomina phenomina, TimeStamp time);
	void TrackPhantom(Phantom key);

	// Kill a player at a given time
	void Kill(int imageIndex, TimeStamp time, const PhenominaHandle& murderHandle, PhenominaHandle& phenominaResetHandle);
	void Revive();
	bool CheckRevive(const PhenominaHandle& resetHandle);
};

