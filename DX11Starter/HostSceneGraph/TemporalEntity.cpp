#include "TemporalEntity.h"


TemporalEntity::TemporalEntity()
{
	m_images = nullptr;
	m_projectileHandles = nullptr;
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
	m_projectileHandles = new PhenominaCreationInfo[maxPhenomina];
	m_phenominaBuffer = new Phenomina[maxPhenomina];

	// Intialize indices
	m_imageCount = 0;
	m_phenominaCount = 0;

	m_handles = handles;

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
	if (m_projectileHandles) 
	{
		delete[] m_projectileHandles;
		m_projectileHandles = nullptr;
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

	m_handles = handles;

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
	if (m_projectileHandles)
	{
		delete[] m_projectileHandles;
		m_projectileHandles = nullptr;
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
	m_projectileHandles = new PhenominaCreationInfo[maxPhenomina];
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

HandleObject TemporalEntity::GetHandle() const 
{
	return m_handles;
}

void TemporalEntity::SetHandle(HandleObject& obj)
{
	m_handles = obj;
}


void TemporalEntity::StackKeyFrame(KeyFrameData keyFrame)
{
	if (m_killedBy.m_entity != -1)
	{
		return;
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

	if (keyFrame.m_shot)
	{
		m_projectileHandles[m_phenominaCount] = PhenominaCreationInfo(m_imageCount, timeStamp);
		// TODO: Check for collisions
		// TODO: Fix handles
		// TODO: MAKE DIFFERENT BULLET TYPES POSSIBLE
		Transform transform = m_images[m_imageCount].GetTransform().GetTransform(keyFrame.m_shotTime);
		const float BULLETRANGE = 10;
		const TimeStamp BULLETPERIOD = 1;
		Vector2 finalPos = Vector2(0, BULLETRANGE).Rotate(-transform.GetRot());
		TimeInstableTransform traj = TimeInstableTransform(transform, Transform(finalPos, transform.GetRot()), keyFrame.m_shotTime, keyFrame.m_shotTime + BULLETPERIOD, false);
		HandleObject bulletHandle = HandleObject();
		bulletHandle.m_material = 1;
		bulletHandle.m_mesh = 1;
		m_phenominaBuffer[m_phenominaCount] = Phenomina(traj, bulletHandle);
		m_phenominaCount++;
	}

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
	m_reversed = m_images[imageIndex].GetTransform().GetReversed();

	// Get the bullet handle to reset to
	phenominaResetHandle = PhenominaHandle(m_entityId, -1);
	for (size_t i = m_phenominaCount - 1; i >= 0; i--)
	{
		if (m_projectileHandles[i].m_imageIndex < imageIndex)
		{
			break;
		}
		phenominaResetHandle.m_phenomina = i;
	}
	// Don't cut a bullet from a timestamp on the image if the death is after it
	if (phenominaResetHandle.m_phenomina != -1 && m_projectileHandles[phenominaResetHandle.m_phenomina].m_imageIndex == imageIndex)
	{
		if (m_reversed) {
			if (m_projectileHandles[phenominaResetHandle.m_phenomina].m_timeStamp > time)
			{
				phenominaResetHandle.m_phenomina++;
			}
		}
		else
		{
			if (m_projectileHandles[phenominaResetHandle.m_phenomina].m_timeStamp < time)
			{
				phenominaResetHandle.m_phenomina++;
			}
		}
	}
}

void TemporalEntity::Revive()
{
	// Most of the work was done by the kill function
	m_killedBy = PhenominaHandle();
}

