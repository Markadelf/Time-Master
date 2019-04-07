#pragma once
#include "Transform.h"
#include "TimeInstableTransform.h"

namespace Colliders2D {

	// Class for handling circular colliders
	class CircleCollider
	{
		float m_radius;
	public:
		CircleCollider(float radius = 0);
		~CircleCollider();

		float GetRadius() const;
	};

	// class for handling rectangular colliders
	class RectangleCollider
	{
		float m_width;
		float m_height;

	public:
		RectangleCollider(float width = 0, float height = 0);
		~RectangleCollider();
	
		float GetWidth() const;
		float GetHeight() const;
	};

	// Collision detection
	// Some functions return more data than others

	// CIRCLE CIRCLE
	// Returns precise collision data with overlap
	float CheckCircleCollisionPrecise(const Vector2& posA, const Vector2& posB, float combineRad);
	// Returns true if collision occurs
	bool CheckCollision(const Vector2& posA, const Vector2& posB, float combineRad);
	// Returns true if collision occurs andd gives overlap vector
	bool CheckCollision(const Transform& posA, const CircleCollider& circleA, const Transform& posB, const CircleCollider& circleB, const Vector2& deltaPos, Vector2& overlap);
	// Returns true if collision occurs
	bool CheckCollision(const Transform& posA, const CircleCollider& circleA, const Transform& posB, const CircleCollider& circleB);

	// CIRCLE BOX
	// Always treats the circle as a box aligned with the surface
	// Returns true if collision occurs and gives an overlap vector
	bool CheckCollision(const Transform& posA, const CircleCollider& circleA, const Transform& posB, const RectangleCollider& rectB, const Vector2& deltaPos, Vector2& overlap);
	// Returns true if collision occurs
	bool CheckCollision(const Transform& posA, const CircleCollider& circleA, const Transform& posB, const RectangleCollider& rectB);

	// TIME INSTABLE COLLISION
	// Returns if the collision occured and the timestamp of the collision
	bool CheckCollision(const TimeInstableTransform& posA, const CircleCollider& circleA, const TimeInstableTransform& posB, const CircleCollider& circleB, TimeStamp& timeStamp);
	bool CheckCollision(const TimeInstableTransform& posA, const CircleCollider& circleA, const Transform& posB, const CircleCollider& circleB, TimeStamp& timeStamp);
	bool CheckCollision(const TimeInstableTransform& posA, const CircleCollider& circleA, const Transform& posB, const RectangleCollider& rectB, TimeStamp& timeStamp);



}