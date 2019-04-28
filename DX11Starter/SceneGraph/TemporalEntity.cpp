#include "TemporalEntity.h"


TemporalEntity::TemporalEntity()
{
	m_images = nullptr;
	m_phenomenaImages = nullptr;
	m_phenomenaBuffer = nullptr;

	m_maxImages = 0;
	m_maxPhenomena = 0;
	m_entityId = -1;
	m_lastTimeStamp = -1;

	m_imageCount = 0;
	m_phenomenaCount = 0;

	// Initialize variables
	m_reversed = false;
}

TemporalEntity::TemporalEntity(int maxImages, int maxPhenomena, const Transform& startingPos, float initialTime, HandleObject handles, int entityId): TemporalEntity()
{
	Initialize(maxImages, maxPhenomena, entityId);
	Initialize(startingPos, initialTime, handles);
}


TemporalEntity::~TemporalEntity()
{
	if (m_images)
	{
		delete[] m_images;
		m_images = nullptr;
	}
	if (m_phenomenaImages) 
	{
		delete[] m_phenomenaImages;
		m_phenomenaImages = nullptr;
	}
	if (m_phenomenaBuffer)
	{
		delete[] m_phenomenaBuffer;
		m_phenomenaBuffer = nullptr;
	}
}

void TemporalEntity::Initialize(const Transform& startingPos, float initialTime, HandleObject handles)
{
	m_currentTransform = startingPos;
	m_lastTimeStamp = initialTime;

	// Intialize indices
	m_imageCount = 0;
	m_phenomenaCount = 0;

	m_handle = handles;

	// Initialize variables
	m_killedBy = PhenomenaHandle();
	m_reversed = false;
}

void TemporalEntity::Initialize(int maxImages, int maxPhenomena, int entityId)
{
	if (m_images)
	{
		delete[] m_images;
		m_images = nullptr;
	}
	if (m_phenomenaImages)
	{
		delete[] m_phenomenaImages;
		m_phenomenaImages = nullptr;
	}
	if (m_phenomenaBuffer)
	{
		delete[] m_phenomenaBuffer;
		m_phenomenaBuffer = nullptr;
	}

	m_maxImages = maxImages;
	m_maxPhenomena = maxPhenomena;
	m_entityId = entityId;

	// Initialize Buffers
	m_images = new Phantom[maxImages];
	m_phenomenaImages = new int[maxPhenomena];
	m_phenomenaBuffer = new Phenomenon[maxPhenomena];

	// Intialize indices
	m_imageCount = 0;
	m_phenomenaCount = 0;
}

PhenomenaHandle TemporalEntity::GetKilledBy()
{
	return m_killedBy;
}

Phantom TemporalEntity::Head()
{
	return m_images[m_imageCount - 1];
}

int TemporalEntity::GetImageCount() const
{
	return m_imageCount;
}

int TemporalEntity::GetPhenomenaCount() const
{
	return m_phenomenaCount;
}

Phantom* TemporalEntity::GetPhantomBuffer() const
{
	return m_images;
}

Phenomenon* TemporalEntity::GetPhenomenaBuffer() const
{
	return m_phenomenaBuffer;
}

Transform TemporalEntity::GetTransform() const
{
	return m_currentTransform;
}

TimeStamp TemporalEntity::GetTimeStamp() const
{
	return m_lastTimeStamp;
}

bool TemporalEntity::GetReversed() const
{
	return m_reversed;
}

ActionInfo TemporalEntity::GetAction() const
{
    return m_action;
}

HandleObject TemporalEntity::GetHandle() const 
{
	return m_handle;
}

void TemporalEntity::SetHandle(HandleObject& obj)
{
	m_handle = obj;
}

void TemporalEntity::SetAction(ActionInfo& action)
{
    m_action = action;
}


Phantom* TemporalEntity::StackKeyFrame(KeyFrameData keyFrame)
{
	if (m_killedBy.m_entity != -1)
	{
		return nullptr;
	}

	TimeStamp timeStamp = keyFrame.m_timeStamp;
	if (timeStamp < m_lastTimeStamp)
	{
		m_images[m_imageCount] = Phantom(TimeInstableTransform(keyFrame.m_transform, m_currentTransform, timeStamp, m_lastTimeStamp, true), keyFrame);
	}
	else
	{
		m_images[m_imageCount] = Phantom(TimeInstableTransform(m_currentTransform, keyFrame.m_transform, m_lastTimeStamp, timeStamp, false), keyFrame);
	}
	m_lastTimeStamp = timeStamp;
	m_currentTransform = keyFrame.m_transform;

	return &m_images[m_imageCount++];
}

void TemporalEntity::TrackPhenomena(Phenomenon phenomena)
{
	m_phenomenaImages[m_phenomenaCount] = m_imageCount;
	m_phenomenaBuffer[m_phenomenaCount] = phenomena;
	m_phenomenaCount++;
}


void TemporalEntity::Kill(int imageIndex, TimeStamp time, const PhenomenaHandle& murderHandle, PhenomenaHandle& phenomenaResetHandle)
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
	m_lastTimeStamp = time;
	m_reversed = m_images[imageIndex].GetTransform().GetReversed();

	// Get the bullet handle to reset to
    phenomenaResetHandle = PhenomenaHandle(m_entityId, -1);
	for (int i = m_phenomenaCount - 1; i >= 0; i--)
	{
		if (m_phenomenaImages[i] < imageIndex)
		{
			break;
		}
        phenomenaResetHandle.m_phenomena = i;
	}
	// Don't cut a bullet from a timestamp on the image if the death is after it
	if (phenomenaResetHandle.m_phenomena != -1 && m_phenomenaImages[phenomenaResetHandle.m_phenomena] == imageIndex)
	{
		if (m_reversed) {
			if (m_images[m_phenomenaImages[phenomenaResetHandle.m_phenomena]].GetTransform().GetEndTime() - m_action.m_deploymentTime > time)
			{
                phenomenaResetHandle.m_phenomena++;
			}
		}
		else
		{
			if (m_images[m_phenomenaImages[phenomenaResetHandle.m_phenomena]].GetTransform().GetStartTime() + m_action.m_deploymentTime < time)
			{
                phenomenaResetHandle.m_phenomena++;
			}
		}
	}
	if (phenomenaResetHandle.m_phenomena != -1)
	{
		m_phenomenaCount = phenomenaResetHandle.m_phenomena;
	}
}

void TemporalEntity::Revive()
{
	// Most of the work was done by the kill function
	m_killedBy = PhenomenaHandle();
}

bool TemporalEntity::CheckRevive(const PhenomenaHandle& resetHandle)
{
	return m_killedBy.m_entity == resetHandle.m_entity && resetHandle.m_phenomena != -1 && resetHandle.m_phenomena <= m_killedBy.m_phenomena;
}

