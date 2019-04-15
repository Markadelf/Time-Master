#include "KeyFrameData.h"


KeyFrameData::KeyFrameData()
{
	m_entityId = -1;
	m_shotTime = -1;
	m_shot = false;
}

KeyFrameData::KeyFrameData(Transform transform, TimeStamp time, int id, bool shot, TimeStamp timeShot)
{
	m_transform = transform;
	m_timeStamp = time;
	m_entityId = id;
	m_shot = shot;
	m_shotTime = timeShot;
}


KeyFrameData::~KeyFrameData()
{
}

bool KeyFrameData::Serialize(Buffer & buffer) const
{
	return
		m_transform.Serialize(buffer) &&
		Serializer::SerializeFloatFP(buffer, (float)m_timeStamp) &&
		Serializer::SerializeInteger<-1, 14>(buffer, m_entityId) &&
		Serializer::SerializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::SerializeFloatFP(buffer, (float)m_shotTime) : true);
}

bool KeyFrameData::Deserialize(Buffer & buffer)
{
	return
		m_transform.Deserialize(buffer) &&
		Serializer::DeserializeFloatFP(buffer, m_timeStamp) &&
		Serializer::DeserializeInteger<-1, 14>(buffer, m_entityId) &&
		Serializer::DeserializeBool(buffer, m_shot) &&
		(m_shot ? Serializer::DeserializeFloatFP(buffer, m_shotTime) : true);
}
