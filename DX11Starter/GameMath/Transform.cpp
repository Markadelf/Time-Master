#include "Transform.h"


Transform::Transform()
{
	m_position = Vector2();
	m_rotation = 0;
}

Transform::Transform(Vector2 pos, float rot)
{
	m_position = pos;
	m_rotation = rot;
}


Transform::~Transform()
{
}

Vector2 Transform::GetPos() const
{
	return m_position;
}

void Transform::SetPos(const Vector2& value)
{
	m_position = value;
}

float Transform::GetRot() const
{
	return m_rotation;
}

void Transform::SetRot(const float& val)
{
	m_rotation = val;
}

bool Transform::Serialize(Buffer& buffer) const
{
	return
		m_position.Serialize(buffer) &&
		Serializer::SerializeFloat<0, 4, 1, 100>(buffer, m_rotation);
}

bool Transform::Deserialize(Buffer& buffer)
{
	return
		m_position.Deserialize(buffer) &&
		Serializer::DeserializeFloat<0, 4, 1, 100>(buffer, m_rotation);
}
