#include "PlayerObject.h"


PlayerObject::PlayerObject() : PlayerObject(2048, 100, Transform(), 0, HandleObject(), -1)
{
}

PlayerObject::PlayerObject(int maxImages, int maxBullets, const Transform& startingPos, float initialTime, HandleObject handles, int playerId)
{
	m_maxImages = maxImages;
	m_maxBullets = maxBullets;
	m_currentTransform = startingPos;
	m_playerId = playerId;
	m_lastTimeStamp = initialTime;

	// Initialize Buffers
	m_images = new Phantom[maxImages];
	m_projectileHandles = new ProjectileLaunchInfo[maxBullets];

	// Intialize indices
	m_imageCount = 0;
	m_projectileCount = 0;

	m_handles = handles;

	// Initialize variables
	m_killedBy = ProjectileHandle();
	m_reversed = false;
}


PlayerObject::~PlayerObject()
{
	delete[] m_images;
	delete[] m_projectileHandles;
}

ProjectileHandle PlayerObject::GetKilledBy()
{
	return m_killedBy;
}

Phantom PlayerObject::Head()
{
	return m_images[m_imageCount - 1];
}

int PlayerObject::GetImageCount() const
{
	return m_imageCount;
}

Phantom* PlayerObject::GetPhantomBuffer() const
{
	return m_images;
}

void PlayerObject::StackKeyFrame(PlayerKeyFrameData keyFrame)
{
	if (m_killedBy.m_playerId != -1)
	{
		return;
	}

	TimeStamp timeStamp = keyFrame.GetTimeStamp();
	if (timeStamp < m_lastTimeStamp)
	{
		m_images[m_imageCount] = Phantom(TimeInstableTransform(keyFrame.GetTransform(), m_currentTransform, timeStamp, m_lastTimeStamp, true), keyFrame);
		m_imageCount++;
	}
	else
	{
		m_images[m_imageCount] = Phantom(TimeInstableTransform(m_currentTransform, keyFrame.GetTransform(), m_lastTimeStamp, timeStamp, false), keyFrame);
		m_imageCount++;
	}
	m_lastTimeStamp = keyFrame.GetTimeStamp();
	m_currentTransform = keyFrame.GetTransform();

	if (keyFrame.GetShot())
	{
		m_projectileHandles[m_projectileCount] = ProjectileLaunchInfo(m_imageCount, timeStamp);
		m_projectileCount++;
	}
}

void PlayerObject::Kill(int imageIndex, TimeStamp time, ProjectileHandle murderHandle, ProjectileHandle& bulletResetHandle)
{
	if (imageIndex > m_imageCount)
	{
		return;
	}
	m_killedBy = murderHandle;

	// Fix image stack
	m_imageCount = imageIndex;
	m_images[imageIndex].Trim(time);
	m_currentTransform = m_images[imageIndex].GetTransform().GetTransform(time);
	m_reversed = m_images[imageIndex].GetTransform().GetReversed();

	// Get the bullet handle to reset to
	bulletResetHandle = ProjectileHandle(m_playerId, -1);
	for (size_t i = m_projectileCount - 1; i >= 0; i--)
	{
		if (m_projectileHandles[i].m_imageIndex < imageIndex)
		{
			break;
		}
		bulletResetHandle.m_bulletId = i;
	}
	// Don't cut a bullet from a timestamp on the image if the death is after it
	if (bulletResetHandle.m_bulletId != -1 && m_projectileHandles[bulletResetHandle.m_bulletId].m_imageIndex == imageIndex)
	{
		if (m_reversed) {
			if (m_projectileHandles[bulletResetHandle.m_bulletId].m_timeStamp > time)
			{
				bulletResetHandle.m_bulletId++;
			}
		}
		else
		{
			if (m_projectileHandles[bulletResetHandle.m_bulletId].m_timeStamp < time)
			{
				bulletResetHandle.m_bulletId++;
			}
		}
	}
}

void PlayerObject::Revive()
{
	// Most of the work was done by the kill function
	m_killedBy = ProjectileHandle();
}

