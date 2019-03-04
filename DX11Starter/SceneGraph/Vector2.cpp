#include "stdafx.h"
#include "Vector2.h"
#include <math.h>

const Vector2 Vector2::Zero = Vector2(0, 0);

Vector2::Vector2()
{
	m_x = 0;
	m_y = 0;
}


Vector2::~Vector2()
{
}

Vector2::Vector2(float x, float y)
{
	m_x = x;
	m_y = y;
}

float Vector2::GetX() const
{
	return m_x;
}

float Vector2::GetY() const
{
	return m_y;
}

Vector2 Vector2::Add(const Vector2 & b)
{
	return Vector2(m_x + b.m_x, m_y + b.m_y);
}

Vector2 Vector2::Subtract(const Vector2 & b)
{
	return Vector2(m_x - b.m_x, m_y - b.m_y);
}

Vector2 Vector2::Multiply(float b)
{
	return Vector2(m_x * b, m_y * b);
}

Vector2 Vector2::Divide(float b)
{
	return Vector2(m_x / b, m_y / b);
}

float Vector2::SquareMagnitude() const
{
	return m_x * m_x + m_y * m_y;
}

Vector2 Vector2::Normalized() const
{
	float mag = sqrtf(SquareMagnitude());
	return Vector2(m_x / mag, m_y / mag);
}

Vector2 Vector2::Rotate(float angle) const
{
	float cosA = cos(angle);
	float sinA = sin(angle);
	return Vector2(m_x * cosA - m_y * sinA, m_y * cosA + m_x * sinA);
}

Vector2 Vector2::Lerp(Vector2 a, Vector2 b, float lerp)
{
	return a + (b - a) * lerp;
}

float Vector2::DotProduct(Vector2 a, Vector2 b)
{
	return a.m_x * b.m_x + a.m_y * b.m_y;
}

bool Vector2::Serialize(Buffer& buffer) const
{
	return
		Serializer::SerializeFloat<-100, 100, 1, 100>(buffer, m_x) &&
		Serializer::SerializeFloat<-100, 100, 1, 100>(buffer, m_y);
}

bool Vector2::Deserialize(Buffer& buffer)
{
	return
		Serializer::DeserializeFloat<-100, 100, 1, 100>(buffer, m_x) &&
		Serializer::DeserializeFloat<-100, 100, 1, 100>(buffer, m_y);
}
