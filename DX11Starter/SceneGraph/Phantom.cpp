#include "Phantom.h"


Phantom::Phantom()
{
	m_entityId = -1;
	m_activeHead = false;
	m_shot = false;
}

Phantom::Phantom(TimeInstableTransform transform, int id, bool activeHead, bool shot, TimeStamp timeShot)
{
	m_transform = transform;
	m_entityId = id;
	m_activeHead = activeHead;
	m_shot = shot;
	m_shotTime = timeShot;
}

Phantom::Phantom(TimeInstableTransform transform, KeyFrameData keyFrame)
{
	m_transform = transform;
	m_entityId = keyFrame.m_entityId;
	m_activeHead = false;
	m_shot = keyFrame.m_shot;
	m_shotTime = keyFrame.m_shotTime;
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
	return m_entityId;
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
	m_transform.Trim(time);
}

bool Phantom::Serialize(Buffer& buffer) const
{
	return 
		m_transform.Serialize(buffer) &&
		Serializer::SerializeInteger<-1, 14>(buffer, m_entityId) &&
		Serializer::SerializeBool(buffer, m_activeHead) &&
		Serializer::SerializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::SerializeFloatFP(buffer, (float)m_shotTime) : true);
}

bool Phantom::Deserialize(Buffer& buffer)
{
	return 
		m_transform.Deserialize(buffer) &&
		Serializer::DeserializeInteger<-1, 14>(buffer, m_entityId) &&
		Serializer::DeserializeBool(buffer, m_activeHead) &&
		Serializer::DeserializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::DeserializeFloatFP(buffer, m_shotTime) : true);
}
