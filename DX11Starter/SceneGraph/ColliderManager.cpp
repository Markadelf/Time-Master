#include "ColliderManager.h"

using namespace Colliders2D;

ColliderManager::ColliderManager()
{
	m_circles = nullptr;
	m_rectangles = nullptr;

	m_maxCircles = 0;
	m_circleCount = 0;

	m_maxRectangles = 0;
	m_rectangleCount = 0;
}

ColliderManager::~ColliderManager()
{
	if (m_circles)
	{
		delete[] m_circles;
		m_circles = nullptr;
	}
	if (m_rectangles)
	{
		delete[] m_rectangles;
		m_rectangles = nullptr;
	}
}

ColliderManager& ColliderManager::get()
{
	static ColliderManager singleton;
	return singleton;
}

void ColliderManager::Reinit(int circles, int rectangles)
{
	if (m_circles)
	{
		delete[] m_circles;
		m_circles = nullptr;
	}
	if (m_rectangles)
	{
		delete[] m_rectangles;
		m_rectangles = nullptr;
	}

	m_circles = new CircleCollider[circles];
	m_rectangles = new RectangleCollider[rectangles];

	m_maxCircles = circles;
	m_circleCount = 0;

	m_maxRectangles = rectangles;
	m_rectangleCount = 0;
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
    // For now we only support LERP Collision Checks in Circular objects
    if (objA.m_type != ColliderType::Circle)
    {
        return false;
    }

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
    // For now we only support LERP Collision Checks in Circular objects
    if (objA.m_type != ColliderType::Circle)
    {
        return false;
    }

	switch (objB.m_type)
	{
	case ColliderType::Circle:
		return Colliders2D::CheckCollision(posA, m_circles[objA.m_handle], posB, m_circles[objB.m_handle], timeStamp);
	case ColliderType::Rectangle:
		return Colliders2D::CheckCollision(posA, m_circles[objA.m_handle], posB, m_rectangles[objB.m_handle], timeStamp);
	default:
		return false;
	}
}
