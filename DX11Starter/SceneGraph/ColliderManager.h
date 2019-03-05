#pragma once
#include "Collider.h"
#include "HandleObject.h"

class ColliderManager
{
	Colliders2D::CircleCollider* m_circles;
	Colliders2D::RectangleCollider* m_rectangles;

	int m_maxCircles;
	int m_circleCount;

	int m_maxRectangles;
	int m_rectangleCount;

public:
	ColliderManager();
	ColliderManager(int circles, int rectangles);
	~ColliderManager();

	// Collider Distribution
	Colliders2D::ColliderHandle GetCircleHandle(float radius);
	Colliders2D::ColliderHandle GetRectangularHandle(float width, float height);

	// Collision Checks
	bool CheckCollision(const Transform& posA, const Colliders2D::ColliderHandle& objA, const Transform& posB, const Colliders2D::ColliderHandle& objB, const Vector2& deltaPos, Vector2& overlap);
	// Returns true if collision occurs
	bool CheckCollision(const Transform& posA, const Colliders2D::ColliderHandle& objA, const Transform& posB, const Colliders2D::ColliderHandle& objB);

	// TIME INSTABLE COLLISION
	// Returns if the collision occured and the timestamp of the collision
	bool CheckCollision(const TimeInstableTransform& posA, const Colliders2D::ColliderHandle& objA, const TimeInstableTransform& posB, const Colliders2D::ColliderHandle& objB, TimeStamp& timeStamp);
};

