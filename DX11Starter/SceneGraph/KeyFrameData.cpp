#include "KeyFrameData.h"


KeyFrameData::KeyFrameData()
{
    m_entityId = -1;
    m_usedAction = false;
}

KeyFrameData::KeyFrameData(Transform transform, TimeStamp time, int id, bool shot)
{
    m_transform = transform;
    m_timeStamp = time;
    m_entityId = id;
    m_usedAction = shot;
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
        Serializer::SerializeBool(buffer, m_usedAction);
}

bool KeyFrameData::Deserialize(Buffer & buffer)
{
    return
        m_transform.Deserialize(buffer) &&
        Serializer::DeserializeFloatFP(buffer, m_timeStamp) &&
        Serializer::DeserializeInteger<-1, 14>(buffer, m_entityId) &&
        Serializer::DeserializeBool(buffer, m_usedAction);
}
