#include "Phantom.h"


Phantom::Phantom()
{
	m_playerId = -1;
	m_activeHead = false;
	m_shot = false;
}

Phantom::Phantom(TimeInstableTransform transform, int id, bool activeHead, bool shot, TimeStamp timeShot)
{
	m_transform = transform;
	m_playerId = id;
	m_activeHead = activeHead;
	m_shot = shot;
	m_shotTime = timeShot;
}

Phantom::Phantom(TimeInstableTransform transform, PlayerKeyFrameData keyFrame)
{
	m_transform = transform;
	m_playerId = keyFrame.GetPlayerId();
	m_activeHead = false;
	m_shot = keyFrame.GetShot();
	m_shotTime = keyFrame.GetShotTime();
}


Phantom::~Phantom()
{
}

TimeInstableTransform Phantom::GetTransform()
{
	return m_transform;
}

int Phantom::GetPlayerId()
{
	return m_playerId;
}

float Phantom::GetShotTime()
{
	return m_shotTime;
}

bool Phantom::IsActiveHead()
{
	return m_activeHead;
}

bool Phantom::GetShot()
{
	return m_shot;
}

void Phantom::Trim(TimeStamp time)
{
	if (m_transform.GetReversed())
	{
		float end = m_transform.GetEndTime();
		m_transform = TimeInstableTransform(m_transform.GetTransform(time), m_transform.GetTransform(end), time, end, true);
	}
	else
	{
		float start = m_transform.GetStartTime();
		m_transform = TimeInstableTransform(m_transform.GetTransform(start), m_transform.GetTransform(time), start, time, false);
	}
}

bool Phantom::Serialize(Buffer& buffer) const
{
	return 
		m_transform.Serialize(buffer) &&
		Serializer::SerializeInteger<-1, 14>(buffer, m_playerId) &&
		Serializer::SerializeBool(buffer, m_activeHead) &&
		Serializer::SerializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::SerializeFloat<-1000, 1000, 1, 30>(buffer, (float)m_shotTime) : true);
}

bool Phantom::Deserialize(Buffer& buffer)
{
	return 
		m_transform.Deserialize(buffer) &&
		Serializer::DeserializeInteger<-1, 14>(buffer, m_playerId) &&
		Serializer::DeserializeBool(buffer, m_activeHead) &&
		Serializer::DeserializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::DeserializeFloat<-1000, 1000, 1, 30>(buffer, m_shotTime) : true);
}
