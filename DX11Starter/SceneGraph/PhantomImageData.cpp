#include "stdafx.h"
#include "PhantomImageData.h"


PhantomImageData::PhantomImageData()
{
	m_playerId = -1;
	m_shotTime = -1;
	m_shot = false;
}

PhantomImageData::PhantomImageData(Transform transform, TimeStamp time, int id, bool shot, TimeStamp timeShot)
{
	m_transform = transform;
	m_timeStamp = time;
	m_playerId = id;
	m_shot = shot;
	m_shotTime = timeShot;
}


PhantomImageData::~PhantomImageData()
{
}

Transform PhantomImageData::GetTransform()
{
	return m_transform;
}

TimeStamp PhantomImageData::GetTimeStamp()
{
	return m_timeStamp;
}

int PhantomImageData::GetPlayerId()
{
	return m_playerId;
}

TimeStamp PhantomImageData::GetShotTime()
{
	return m_shotTime;
}

bool PhantomImageData::GetShot()
{
	return m_shot;
}

bool PhantomImageData::Serialize(Buffer & buffer) const
{
	return
		m_transform.Serialize(buffer) &&
		Serializer::SerializeFloat<-1000, 1000, 1, 30>(buffer, (float)m_timeStamp) &&
		Serializer::SerializeInteger<-1, 14>(buffer, m_playerId) &&
		Serializer::SerializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::SerializeFloat<-1000, 1000, 1, 30>(buffer, (float)m_shotTime) : true);
}

bool PhantomImageData::Deserialize(Buffer & buffer)
{
	return
		m_transform.Deserialize(buffer) &&
		Serializer::DeserializeFloat<-1000, 1000, 1, 30>(buffer, m_timeStamp) &&
		Serializer::DeserializeInteger<-1, 14>(buffer, m_playerId) &&
		Serializer::DeserializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::DeserializeFloat<-1000, 1000, 1, 30>(buffer, m_shotTime) : true);
}
