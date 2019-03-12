#include "TimeInstableTransform.h"


TimeInstableTransform::TimeInstableTransform()
{
	m_start = Transform();
	m_end = Transform();
	m_startTime = -1;
	m_endTime = -1;
	m_reversed = false;
}

TimeInstableTransform::TimeInstableTransform(Transform start, Transform end, TimeStamp startTime, TimeStamp endTime, bool reversed)
{
	m_start = start;
	m_end = end;
	m_startTime = startTime;
	m_endTime = endTime;
	m_reversed = reversed;
}

TimeInstableTransform::~TimeInstableTransform()
{
}

Transform TimeInstableTransform::GetTransform(TimeStamp timeStamp) const
{
	if (m_endTime == m_startTime) {
		return m_start;
	}
	return Transform(GetPos(timeStamp), GetRot(timeStamp));
}

Vector2 TimeInstableTransform::GetPos(TimeStamp timeStamp) const
{
	if (m_endTime == m_startTime) {
		return m_start.GetPos();
	}
	return Vector2::Lerp(m_start.GetPos(), m_end.GetPos(), (timeStamp - m_startTime) / (m_endTime - m_startTime));
}

float TimeInstableTransform::GetRot(TimeStamp timeStamp) const
{
	if (m_endTime == m_startTime) {
		return m_start.GetRot();
	}
	return m_start.GetRot() + (m_end.GetRot() - m_start.GetRot()) * (timeStamp - m_startTime) / (m_endTime - m_startTime);
}

TimeStamp TimeInstableTransform::GetStartTime() const
{
	return m_startTime;
}

TimeStamp TimeInstableTransform::GetEndTime() const
{
	return m_endTime;
}

bool TimeInstableTransform::GetReversed() const
{
	return m_reversed;
}

bool TimeInstableTransform::Serialize(Buffer& buffer) const
{
	return 
		m_start.Serialize(buffer) && 
		m_end.Serialize(buffer) &&
		Serializer::SerializeFloat<-1000, 1000, 1, 30>(buffer, (float)m_startTime) &&
		Serializer::SerializeFloat<-1000, 1000, 1, 30>(buffer, (float)m_endTime) && 
		Serializer::SerializeBool(buffer, m_reversed);
}

bool TimeInstableTransform::Deserialize(Buffer& buffer)
{
	return
		m_start.Deserialize(buffer) &&
		m_end.Deserialize(buffer) &&
		Serializer::DeserializeFloat<-1000, 1000, 1, 30>(buffer, m_startTime) &&
		Serializer::DeserializeFloat<-1000, 1000, 1, 30>(buffer, m_endTime) &&
		Serializer::DeserializeBool(buffer, m_reversed);
}
