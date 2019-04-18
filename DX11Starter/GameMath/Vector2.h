#pragma once
#include <Serializer.h>

struct Vector2
{
	const static Vector2 Zero;
	float m_x;
	float m_y;
public:
	Vector2();
	~Vector2();
	Vector2(float x, float y);

	// Accessors
	float GetX() const;
	float GetY() const;

	// Simple math
	Vector2 Add(const Vector2& b);
	Vector2 Subtract(const Vector2& b);
	Vector2 Multiply(float b);
	Vector2 Divide(float b);

	// More complex math
	float SquareMagnitude() const;
	Vector2 Normalized() const;

	// Rotatation Math
	Vector2 Rotate(float angle) const;

	// SLERP Math is static to avoid confusion about which is a or b
	static Vector2 Lerp(Vector2 a, Vector2 b, float lerp);
	static float DotProduct(Vector2 a, Vector2 b);

	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

static Vector2 operator+(Vector2 a, Vector2 b) {
	return a.Add(b);
}

static Vector2 operator-(Vector2 a, Vector2 b) {
	return a.Subtract(b);
}

static Vector2 operator*(Vector2 a, float b) {
	return a.Multiply(b);
}

static Vector2 operator*(float a, Vector2 b) {
	return b.Multiply(a);
}

static Vector2 operator/(Vector2 a, float b) {
	return a.Divide(b);
}

static Vector2 operator/(float a, Vector2 b) {
	return b.Divide(a);
}
