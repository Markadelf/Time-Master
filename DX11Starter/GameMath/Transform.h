#pragma once

#include "Vector2.h"
#include <Serializer.h>

class Transform
{
	Vector2 m_position;
	float m_rotation;
	//float Scale?

public:
	Transform();
	Transform(Vector2 pos, float rot);
	~Transform();

	// Accessors
	Vector2 GetPos() const;
	void SetPos(const Vector2& value);

	float GetRot() const;
	void SetRot(const float& val);

	// Serialization
	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

