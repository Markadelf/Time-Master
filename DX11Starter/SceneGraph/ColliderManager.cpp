#include "ColliderManager.h"

using namespace Colliders2D;

ColliderManager::ColliderManager() : ColliderManager(16, 16)
{
}

ColliderManager::ColliderManager(int circles, int rectangles)
{
	m_circles = new CircleCollider[circles];
	m_rectangles = new RectangleCollider[rectangles];

	m_maxCircles = circles;
	m_circleCount = 0;

	m_maxRectangles = rectangles;
	m_rectangleCount = 0;
}


ColliderManager::~ColliderManager()
{
	delete[] m_circles;
	delete[] m_rectangles;
}

Colliders2D::ColliderHandle ColliderManager::GetCircleHandle(float radius)
{
	for (int i = 0; i < m_circleCount; i++)
	{
		if (m_circles[i].GetRadius() == radius)
		{
			return ColliderHandle(ColliderType::Circle, i);
		}
	}
	if (m_circleCount < m_maxCircles) {
		m_circles[m_circleCount] = CircleCollider(radius);
		return ColliderHandle(ColliderType::Circle, m_circleCount++);
	}
	// Overflow
	return Colliders2D::ColliderHandle();
}

Colliders2D::ColliderHandle ColliderManager::GetRectangularHandle(float width, float height)
{
	for (int i = 0; i < m_rectangleCount; i++)
	{
		if (m_rectangles[i].GetHeight() == height && m_rectangles[i].GetWidth() == width)
		{
			return ColliderHandle(ColliderType::Rectangle, i);
		}
	}
	if (m_rectangleCount < m_maxRectangles) {
		m_rectangles[m_rectangleCount] = RectangleCollider(width, height);
		return ColliderHandle(ColliderType::Rectangle, m_rectangleCount++);
	}
	return Colliders2D::ColliderHandle();
}


// For now we only support movement in Circular objects, so we assume the handle for objA is a circle handle
bool ColliderManager::CheckCollision(const Transform& posA, const Colliders2D::ColliderHandle& objA, const Transform & posB, const Colliders2D::ColliderHandle& objB, const Vector2& deltaPos, Vector2& overlap)
{
	switch (objB.m_type)
	{
	case ColliderType::Circle:
		return Colliders2D::CheckCollision(posA, m_circles[objA.m_handle], posB, m_circles[objB.m_handle], deltaPos, overlap);
	case ColliderType::Rectangle:
		return Colliders2D::CheckCollision(posA, m_circles[objA.m_handle], posB, m_rectangles[objB.m_handle], deltaPos, overlap);
	default:
		return false;
	}
}

bool ColliderManager::CheckCollision(const Transform& posA, const Colliders2D::ColliderHandle& objA, const Transform& posB, const Colliders2D::ColliderHandle& objB)
{
	switch (objB.m_type)
	{
	case ColliderType::Circle:
		return Colliders2D::CheckCollision(posA, m_circles[objA.m_handle], posB, m_circles[objB.m_handle]);
	case ColliderType::Rectangle:
		return Colliders2D::CheckCollision(posA, m_circles[objA.m_handle], posB, m_rectangles[objB.m_handle]);
	default:
		return false;
	}
}

bool ColliderManager::CheckCollision(const TimeInstableTransform& posA, const Colliders2D::ColliderHandle& objA, const TimeInstableTransform& posB, const Colliders2D::ColliderHandle& objB, TimeStamp& timeStamp)
{
	switch (objB.m_type)
	{
		// For now we only support LERP Collision Checks in Circular objects
	case ColliderType::Circle:
		return Colliders2D::CheckCollision(posA, m_circles[objA.m_handle], posB, m_circles[objB.m_handle], timeStamp);
	default:
		return false;
	}
}

bool ColliderManager::CheckCollision(const TimeInstableTransform& posA, const Colliders2D::ColliderHandle& objA, const Transform& posB, const Colliders2D::ColliderHandle& objB, TimeStamp& timeStamp)
{
	switch (objB.m_type)
	{
		// For now we only support LERP Collision Checks in Circular objects
	case ColliderType::Circle:
		return Colliders2D::CheckCollision(posA, m_circles[objA.m_handle], posB, m_circles[objB.m_handle], timeStamp);
	case ColliderType::Rectangle:
		return Colliders2D::CheckCollision(posA, m_circles[objA.m_handle], posB, m_rectangles[objB.m_handle], timeStamp);
	default:
		return false;
	}
}
