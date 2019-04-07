#include "TemporalEntity.h"


TemporalEntity::TemporalEntity()
{
	m_images = nullptr;
	m_phenominaHandles = nullptr;
	m_phenominaBuffer = nullptr;

	m_maxImages = 0;
	m_maxPhenomina = 0;
	m_entityId = -1;
	m_lastTimeStamp = -1;

	m_imageCount = 0;
	m_phenominaCount = 0;
}

TemporalEntity::TemporalEntity(int maxImages, int maxPhenomina, const Transform& startingPos, float initialTime, HandleObject handles, int entityId)
{
	m_maxImages = maxImages;
	m_maxPhenomina = maxPhenomina;
	m_currentTransform = startingPos;
	m_entityId = entityId;
	m_lastTimeStamp = initialTime;

	// Initialize Buffers
	m_images = new Phantom[maxImages];
	m_phenominaHandles = new PhenominaCreationInfo[maxPhenomina];
	m_phenominaBuffer = new Phenomina[maxPhenomina];

	// Intialize indices
	m_imageCount = 0;
	m_phenominaCount = 0;

	m_handle = handles;

	// Initialize variables
	m_killedBy = PhenominaHandle();
	m_reversed = false;
}


TemporalEntity::~TemporalEntity()
{
	if (m_images)
	{
		delete[] m_images;
		m_images = nullptr;
	}
	if (m_phenominaHandles) 
	{
		delete[] m_phenominaHandles;
		m_phenominaHandles = nullptr;
	}
	if (m_phenominaBuffer)
	{
		delete[] m_phenominaBuffer;
		m_phenominaBuffer = nullptr;
	}
}

void TemporalEntity::Initialize(const Transform& startingPos, float initialTime, HandleObject handles)
{
	m_currentTransform = startingPos;
	m_lastTimeStamp = initialTime;

	// Intialize indices
	m_imageCount = 0;
	m_phenominaCount = 0;

	m_handle = handles;

	// Initialize variables
	m_killedBy = PhenominaHandle();
	m_reversed = false;
}

void TemporalEntity::Initialize(int maxImages, int maxPhenomina, int entityId)
{
	if (m_images)
	{
		delete[] m_images;
		m_images = nullptr;
	}
	if (m_phenominaHandles)
	{
		delete[] m_phenominaHandles;
		m_phenominaHandles = nullptr;
	}
	if (m_phenominaBuffer)
	{
		delete[] m_phenominaBuffer;
		m_phenominaBuffer = nullptr;
	}

	m_maxImages = maxImages;
	m_maxPhenomina = maxPhenomina;
	m_entityId = entityId;

	// Initialize Buffers
	m_images = new Phantom[maxImages];
	m_phenominaHandles = new PhenominaCreationInfo[maxPhenomina];
	m_phenominaBuffer = new Phenomina[maxPhenomina];

	// Intialize indices
	m_imageCount = 0;
	m_phenominaCount = 0;
}

PhenominaHandle TemporalEntity::GetKilledBy()
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

int TemporalEntity::GetPhenominaCount() const
{
	return m_phenominaCount;
}

Phantom* TemporalEntity::GetPhantomBuffer() const
{
	return m_images;
}

Phenomina* TemporalEntity::GetPhenominaBuffer() const
{
	return m_phenominaBuffer;
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

HandleObject TemporalEntity::GetHandle() const 
{
	return m_handle;
}

void TemporalEntity::SetHandle(HandleObject& obj)
{
	m_handle = obj;
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

void TemporalEntity::TrackPhenomina(Phenomina phenomina, TimeStamp time)
{
	m_phenominaHandles[m_phenominaCount] = PhenominaCreationInfo(m_imageCount, time);
	m_phenominaBuffer[m_phenominaCount] = phenomina;
	m_phenominaCount++;
}

void TemporalEntity::TrackPhantom(Phantom key)
{
	m_images[m_imageCount] = key;
	if (key.GetTransform().GetReversed())
	{
		m_lastTimeStamp = key.GetTransform().GetStartTime();
	}
	else
	{
		m_lastTimeStamp = key.GetTransform().GetEndTime();
	}

	m_currentTransform = key.GetTransform().GetTransform(m_lastTimeStamp);
	m_imageCount++;
}


void TemporalEntity::Kill(int imageIndex, TimeStamp time, const PhenominaHandle& murderHandle, PhenominaHandle& phenominaResetHandle)
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
	phenominaResetHandle = PhenominaHandle(m_entityId, -1);
	for (int i = m_phenominaCount - 1; i >= 0; i--)
	{
		if (m_phenominaHandles[i].m_imageIndex < imageIndex)
		{
			break;
		}
		phenominaResetHandle.m_phenomina = i;
	}
	// Don't cut a bullet from a timestamp on the image if the death is after it
	if (phenominaResetHandle.m_phenomina != -1 && m_phenominaHandles[phenominaResetHandle.m_phenomina].m_imageIndex == imageIndex)
	{
		if (m_reversed) {
			if (m_phenominaHandles[phenominaResetHandle.m_phenomina].m_timeStamp > time)
			{
				phenominaResetHandle.m_phenomina++;
			}
		}
		else
		{
			if (m_phenominaHandles[phenominaResetHandle.m_phenomina].m_timeStamp < time)
			{
				phenominaResetHandle.m_phenomina++;
			}
		}
	}
	if (phenominaResetHandle.m_phenomina != -1)
	{
		m_phenominaCount = phenominaResetHandle.m_phenomina;
	}
}

void TemporalEntity::Revive()
{
	// Most of the work was done by the kill function
	m_killedBy = PhenominaHandle();
}

bool TemporalEntity::CheckRevive(const PhenominaHandle& resetHandle)
{
	return m_killedBy.m_entity == resetHandle.m_entity && resetHandle.m_phenomina != -1 && resetHandle.m_phenomina <= m_killedBy.m_phenomina;
}

