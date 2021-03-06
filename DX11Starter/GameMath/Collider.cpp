#include "Collider.h"
#include <math.h>

using namespace Colliders2D;

// CIRCLE
CircleCollider::CircleCollider(float radius) : m_radius(radius)
{
}

CircleCollider::~CircleCollider()
{
}

float CircleCollider::GetRadius() const
{
	return m_radius;
}

// RECTANGLE
RectangleCollider::RectangleCollider(float width, float height) : m_width(width), m_height(height)
{
}

Colliders2D::RectangleCollider::~RectangleCollider()
{
}

float Colliders2D::RectangleCollider::GetWidth() const
{
	return m_width;
}

float Colliders2D::RectangleCollider::GetHeight() const
{
	return m_height;
}

// COLLISION CHECKS
float Colliders2D::CheckCircleCollisionPrecise(const Vector2& posA, const Vector2& posB, float combineRad)
{
	return combineRad - sqrtf((posB - posA).SquareMagnitude());
}

bool Colliders2D::CheckCollision(const Vector2& posA, const Vector2& posB, float combineRad)
{
	return combineRad * combineRad > (posB - posA).SquareMagnitude();
}

bool Colliders2D::CheckCollision(const Transform& posA, const CircleCollider& circleA, const Transform& posB, const CircleCollider& circleB, const Vector2& deltaPos, Vector2& overlap)
{
	Vector2 realPosA = posA.GetPos() + deltaPos;
	Vector2 delta = posB.GetPos() - realPosA;
	float mag = (delta).SquareMagnitude();
	float combineRad = circleA.GetRadius() + circleB.GetRadius();
	if (mag < combineRad * combineRad)
	{
		mag = sqrtf(mag);
		overlap = (delta / mag) * (combineRad - mag) * -1;
		return true;
	}
	return false;
}

bool Colliders2D::CheckCollision(const Transform& posA, const CircleCollider& circleA, const Transform& posB, const CircleCollider& circleB)
{
	return CheckCollision(posA.GetPos(), posB.GetPos(), circleA.GetRadius() + circleB.GetRadius());
}

bool Colliders2D::CheckCollision(const Transform& posA, const CircleCollider& circleA, const Transform& posB, const RectangleCollider& rectB, const Vector2& deltaPos, Vector2& overlap)
{
	Vector2 circleRelPos = (posA.GetPos() - posB.GetPos()).Rotate(posB.GetRot());
	float effectiveWidth = circleA.GetRadius() + rectB.GetWidth();
	float effectiveHeight = circleA.GetRadius() + rectB.GetHeight();
	float overlapX = effectiveWidth - fabsf(circleRelPos.GetX());
	float overlapY = effectiveHeight - fabsf(circleRelPos.GetY());
	if (overlapX > 0 && overlapY > 0)
	{
		// We return an overlap vector parallel to the normal of the surface we are skimming
		if (overlapX < overlapY)
		{
			overlap = Vector2(circleRelPos.GetX() / fabsf(circleRelPos.GetX()) * overlapX, 0);
		}
		else
		{
			overlap = Vector2(0, circleRelPos.GetY() / fabsf(circleRelPos.GetY()) * overlapY);
		}
		overlap = overlap.Rotate(-posB.GetRot());
		return true;
	}
	return false;
}

bool Colliders2D::CheckCollision(const Transform& posA, const CircleCollider& circleA, const Transform& posB, const RectangleCollider& rectB)
{
	Vector2 circleRelPos = (posA.GetPos() - posB.GetPos()).Rotate(posB.GetRot());
	float effectiveWidth = circleA.GetRadius() + rectB.GetWidth();
	float effectiveHeight = circleA.GetRadius() + rectB.GetHeight();
	return fabsf(circleRelPos.GetX()) < effectiveWidth && fabsf(circleRelPos.GetY()) < effectiveHeight;
}

bool Colliders2D::CheckCollision(const TimeInstableTransform& posA, const CircleCollider& circleA, const TimeInstableTransform& posB, const CircleCollider& circleB, TimeStamp& timeStamp)
{
	TimeStamp tI = (TimeStamp)posA.GetStartTime() > posB.GetStartTime() ? posA.GetStartTime() : posB.GetStartTime();
	TimeStamp tF = (TimeStamp)posA.GetEndTime() < posB.GetEndTime() ? posA.GetEndTime() : posB.GetEndTime();

	Vector2 aStart = posA.GetPos(tI);
	Vector2 bStart = posB.GetPos(tI);

	Vector2 aFinal = posA.GetPos(tF);
	Vector2 bFinal = posB.GetPos(tF);

	Vector2 relVel = (aFinal - aStart + bStart - bFinal) / (tF - tI);
	Vector2 aRelB = aStart - bStart;

	// Our friend the dot product
	TimeStamp t = tI + (TimeStamp)-(Vector2::DotProduct(relVel, aRelB)) / relVel.SquareMagnitude();
	if (t < tI)
	{
		if (CheckCollision(posA.GetPos(tI), posB.GetPos(tI), circleA.GetRadius() + circleB.GetRadius()))
		{
			timeStamp = tI;
			return true;
		}
	}
	else if (t > tF)
	{
		if (CheckCollision(posA.GetPos(tF), posB.GetPos(tF), circleA.GetRadius() + circleB.GetRadius()))
		{
			timeStamp = tF;
			return true;
		}
	}
	else if (CheckCollision(posA.GetPos(t), posB.GetPos(t), circleA.GetRadius() + circleB.GetRadius())) {
		timeStamp = t;
		return true;
	}
	return false;
}

bool Colliders2D::CheckCollision(const TimeInstableTransform& posA, const CircleCollider& circleA, const Transform& posB, const CircleCollider& circleB, TimeStamp& timeStamp)
{
	TimeStamp tI = posA.GetStartTime();
	TimeStamp tF = posA.GetEndTime();

	Vector2 aStart = posA.GetPos(tI);
	Vector2 bPos = posB.GetPos();

	Vector2 aFinal = posA.GetPos(tF);

	Vector2 relVel = (aFinal - aStart) / (tF - tI);
	Vector2 aRelB = aStart - bPos;
	float sqrMag = relVel.SquareMagnitude();
	if (sqrMag == 0)
	{
		if (CheckCollision(posA.GetPos(tI), bPos, circleA.GetRadius() + circleB.GetRadius()))
		{
			timeStamp = posA.GetReversed() ? tF : tI;
			return true;
		}
	}

	// Our friend the dot product
	TimeStamp t = tI + (TimeStamp)-(Vector2::DotProduct(relVel, aRelB)) / sqrMag;
	if (t < tI)
	{
		if (CheckCollision(posA.GetPos(tI), bPos, circleA.GetRadius() + circleB.GetRadius()))
		{
			timeStamp = tI;
			return true;
		}
	}
	else if (t > tF)
	{
		if (CheckCollision(posA.GetPos(tF), bPos, circleA.GetRadius() + circleB.GetRadius()))
		{
			timeStamp = tF;
			return true;
		}
	}
	else if (CheckCollision(posA.GetPos(t), bPos, circleA.GetRadius() + circleB.GetRadius())) {
		timeStamp = t;
		return true;
	}
	return false;
}

bool Colliders2D::CheckCollision(const TimeInstableTransform& posA, const CircleCollider& circleA, const Transform& posB, const RectangleCollider& rectB, TimeStamp& timeStamp)
{
	TimeStamp tI = posA.GetStartTime();
	TimeStamp tF = posA.GetEndTime();

	Vector2 aStart = posA.GetPos(tI);
	Vector2 bPos = posB.GetPos();

	Vector2 aFinal = posA.GetPos(tF);

	Vector2 relVel = (aFinal - aStart) / (tF - tI);
	Vector2 aRelB = aStart - bPos;
	float sqrMag = relVel.SquareMagnitude();
	if (sqrMag == 0)
	{
		if (CheckCollision(posA.GetTransform(tI), circleA, posB, rectB))
		{
			timeStamp = posA.GetReversed() ? tF : tI;
			return true;
		}
	}

	Vector2 axis[2] = {
		Vector2(rectB.GetWidth() + circleA.GetRadius(), 0).Rotate(posB.m_rotation),
		Vector2(0, rectB.GetHeight() + circleA.GetRadius()).Rotate(posB.m_rotation),
	};

	// Our friend the dot product
	TimeStamp t[5] = { 
		// Time closest to the center
		tI + (TimeStamp)-(Vector2::DotProduct(relVel, aRelB)) / sqrMag,
		tI + (TimeStamp)-(Vector2::DotProduct(relVel, aRelB + axis[0] + axis[1])) / sqrMag,
		tI + (TimeStamp)-(Vector2::DotProduct(relVel, aRelB + axis[0] - axis[1])) / sqrMag,\
		tI + (TimeStamp)-(Vector2::DotProduct(relVel, aRelB - axis[0] + axis[1])) / sqrMag,
		tI + (TimeStamp)-(Vector2::DotProduct(relVel, aRelB - axis[0] - axis[1])) / sqrMag,
	};
	for (size_t i = 0; i < 5; i++)
	{
		Vector2 delta;
		Vector2 overlap;

		if (t[i] > tI && t[i] < tF && CheckCollision(posA.GetTransform(t[i]), circleA, posB, rectB, delta, overlap))
		{
			timeStamp = t[i] - std::sqrt(overlap.SquareMagnitude() / relVel.SquareMagnitude());
			return true;
		}
	}
	if (CheckCollision(posA.GetTransform(tI), circleA, posB, rectB))
	{
		timeStamp = tI;
		return true;
	}
	if (CheckCollision(posA.GetTransform(tF), circleA, posB, rectB))
	{
		timeStamp = tF;
		return true;
	}
	return false;
}

