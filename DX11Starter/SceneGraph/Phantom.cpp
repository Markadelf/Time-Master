#include "Phantom.h"


Phantom::Phantom()
{
	m_entityId = -1;
	m_shot = false;
}

Phantom::Phantom(TimeInstableTransform transform, int id, bool shot)
{
	m_transform = transform;
	m_entityId = id;
	m_shot = shot;
}

Phantom::Phantom(TimeInstableTransform transform, KeyFrameData keyFrame)
{
	m_transform = transform;
	m_entityId = keyFrame.m_entityId;
	m_shot = keyFrame.m_usedAction;
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
        Serializer::SerializeBool(buffer, m_shot);;
}

bool Phantom::Deserialize(Buffer& buffer)
{
    return
        m_transform.Deserialize(buffer) &&
        Serializer::DeserializeInteger<-1, 14>(buffer, m_entityId) &&
        Serializer::DeserializeBool(buffer, m_shot);
}

#ifdef CLIENT
void Phantom::SetPersonalTime(float val)
{
    m_personalTime = val;
}
float Phantom::GetPersonalTime()
{
    return m_personalTime;
}
#endif // CLIENT
