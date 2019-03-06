#include "PlayerKeyFrameData.h"


PlayerKeyFrameData::PlayerKeyFrameData()
{
	m_playerId = -1;
	m_shotTime = -1;
	m_shot = false;
}

PlayerKeyFrameData::PlayerKeyFrameData(Transform transform, TimeStamp time, int id, bool shot, TimeStamp timeShot)
{
	m_transform = transform;
	m_timeStamp = time;
	m_playerId = id;
	m_shot = shot;
	m_shotTime = timeShot;
}


PlayerKeyFrameData::~PlayerKeyFrameData()
{
}

Transform PlayerKeyFrameData::GetTransform()
{
	return m_transform;
}

TimeStamp PlayerKeyFrameData::GetTimeStamp()
{
	return m_timeStamp;
}

int PlayerKeyFrameData::GetPlayerId()
{
	return m_playerId;
}

TimeStamp PlayerKeyFrameData::GetShotTime()
{
	return m_shotTime;
}

bool PlayerKeyFrameData::GetShot()
{
	return m_shot;
}

bool PlayerKeyFrameData::Serialize(Buffer & buffer) const
{
	return
		m_transform.Serialize(buffer) &&
		Serializer::SerializeFloat<-1000, 1000, 1, 30>(buffer, (float)m_timeStamp) &&
		Serializer::SerializeInteger<-1, 14>(buffer, m_playerId) &&
		Serializer::SerializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::SerializeFloat<-1000, 1000, 1, 30>(buffer, (float)m_shotTime) : true);
}

bool PlayerKeyFrameData::Deserialize(Buffer & buffer)
{
	return
		m_transform.Deserialize(buffer) &&
		Serializer::DeserializeFloat<-1000, 1000, 1, 30>(buffer, m_timeStamp) &&
		Serializer::DeserializeInteger<-1, 14>(buffer, m_playerId) &&
		Serializer::DeserializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::DeserializeFloat<-1000, 1000, 1, 30>(buffer, m_shotTime) : true);
}
