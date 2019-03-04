#pragma once
#include "TimeInstableTransform.h"
#include "HandleObject.h"
#include "ProjectileManager.h"

// The player object keeps a record of all of the actions a player has taken
class PlayerObject
{
	struct ProjectileLaunchInfo
	{
		// Time stamp of shot as it corresponds to the INDEX OF THE IMAGE
		int m_imageIndex;

		// Time Stamp of the shot in GAME TIME
		TimeStamp m_timeStamp;
		ProjectileLaunchInfo() 
		{
			m_imageIndex = -1;
			m_timeStamp = 0;
		}

		ProjectileLaunchInfo(int image, TimeStamp time) {
			m_imageIndex = image;
			m_timeStamp = time;
		}
	};

	// Stack of positions at various times
	TimeInstableTransform* m_images;
	// Buffer containing indexes of projectiles
	ProjectileLaunchInfo* m_projectileHandles;

	// Most Recent Transform of the Player
	Transform m_currentTransform;

	// Handle object that includes info for rendering and collisions
	HandleObject m_handles;

	// Buffer Sizes
	int m_maxImages;
	int m_maxBullets;

	// Current Head of the time stack
	int m_imageCount;
	int m_projectileCount;

	// Other Variables
	ProjectileHandle m_killedBy;	// -1 if alive
	bool m_reversed;				// false if moving forward in time
	int m_playerId;
	float m_lastTimeStamp;

public:
	PlayerObject();
	PlayerObject(int maxImages, int maxBullets, const Transform& startingPos, float initialTime, HandleObject handles, int playerId);
	~PlayerObject();

	// Accessor functions
	ProjectileHandle GetKilledBy();
	TimeInstableTransform Head();

	// Upkeep/Update functions
	void TrackMovement(Transform newTrans, TimeStamp timeStamp);
	void TrackBulletShot(TimeStamp timeStamp);


	// Kill a player at a given time
	void Kill(int imageIndex, TimeStamp time, ProjectileHandle murderHandle, ProjectileHandle& bulletResetHandle);
	void Revive();
};

