#include "Player.h"
#include "ColliderManager.h"

Player::Player()
{
	m_reversed = false;
}


Player::~Player()
{
}

void Player::Initialize(const Transform& startingPos, float initialTime, HandleObject handle)
{
	m_transform = startingPos;
	m_time = initialTime;
	m_handle = handle;
}

void Player::Update(float deltaTime)
{
	m_time += deltaTime * (m_reversed ? -1 : 1);

	UpdatePosition(deltaTime);

	// TODO: Replace with other input logic
	static bool rHeld = false;
	if (GetAsyncKeyState('R') & 0x8000)
	{
		if (!rHeld)
		{
			m_reversed = !m_reversed;
			rHeld = true;
		}
	}
	else
	{
		rHeld = false;
	}
	static bool held = false;
	if (GetAsyncKeyState(' ') & 0x8000)
	{
		if (!held)
		{
			m_lastTimeShot = m_time;
			m_shot = true;
			held = true;
		}
	}
	else
	{
		held = false;
	}
}

Transform Player::GetTransform() const
{
	return m_transform;
}

TimeStamp Player::GetTimeStamp() const
{
	return m_time;
}

TimeStamp Player::GetTimeShot() const
{
	return m_lastTimeShot;
}

bool Player::GetReversed() const
{
	return m_reversed;
}

int Player::GetEntityId() const
{
	return m_entityId;
}

HandleObject Player::GetHandle() const
{
	return m_handle;
}

void Player::SetHandle(HandleObject& obj)
{
	m_handle = obj;
}

void Player::Rotate(float amount)
{
	m_transform.SetRot(m_transform.GetRot() + amount);
}

void Player::SetEntityId(int id)
{
	m_entityId = id;
}

void Player::SetTransform(Transform trans)
{
	m_transform = trans;
}

KeyFrameData Player::GetKeyFrame()
{
	KeyFrameData key = KeyFrameData(m_transform, m_time, m_entityId, m_shot, m_lastTimeShot);
	// Reset shot info
	m_shot = false;
	return key;
}

void Player::UpdatePosition(float deltaTime) {
	Vector2 vel;
	// TODO: Replace with input logic
	if (GetAsyncKeyState('W') & 0x8000)
	{
		vel = vel + Vector2(0, 1);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		vel = vel + Vector2(-1, 0);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		vel = vel + Vector2(0, -1);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		vel = vel + Vector2(1, 0);
	}
	vel = vel.Rotate(-m_transform.GetRot());
	m_transform.SetPos(m_transform.GetPos() + vel * deltaTime);
}