#include "Player.h"
#include "ColliderManager.h"
#include "GameUI.h"

Player::Player()
{
	m_reversed = false;
    m_keyFrameRequested = true;
    m_reportActionUsed = false;
    m_usedAction = false;
}


Player::~Player()
{
}

void Player::Initialize(const Transform& startingPos, float initialTime, HandleObject handle, float keyPeriod)
{
	m_transform = startingPos;
	m_time = initialTime;
	m_handle = handle;
    m_keyPeriod = keyPeriod;
    m_keyFrameTimer = 0;
    m_dead = false;
}

void Player::Reposition(const Transform& pos, float time)
{
	m_transform = pos;
	m_time = time;
}

void Player::Update(float deltaTime)
{
	if (!m_dead)
	{
		m_time += deltaTime * (m_reversed ? -1 : 1);
		UpdatePosition(deltaTime);

		if (!m_usedAction)
		{
			m_keyFrameTimer -= deltaTime;
			if (m_keyFrameTimer <= 0)
			{
				m_keyFrameRequested = true;
			}

			// TODO: Replace with other input logic
			static bool rHeld = false;
			if (GetAsyncKeyState('R') & 0x8000)
			{
				if (!rHeld)
				{
					m_reversed = !m_reversed;
					rHeld = true;
					m_keyFrameRequested = true;
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
					m_actionUsedTime = m_time;
					m_keyFrameRequested = true;
					m_usedAction = true;
					m_reversed = false;
					held = true;
				}
			}
			else
			{
				held = false;
			}
		}
		else
		{
			if (m_time > m_actionUsedTime + m_action.m_duration)
			{
				m_usedAction = false;
				m_keyFrameRequested = true;
				m_reportActionUsed = true;
			}
		}
	}
	else 
	{
		m_keyFrameTimer -= deltaTime;
		if (m_keyFrameTimer <= 0)
		{
			m_keyFrameRequested = true;
		}
	}

    GameUI::Get().UpdateGameUI(m_dead, m_time);
}

Transform Player::GetTransform() const
{
	return m_transform;
}

TimeStamp Player::GetTimeStamp() const
{
	return m_time;
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

void Player::SetAction(ActionInfo action)
{
    m_action = action;
}

void Player::SetDead(bool val)
{
	m_dead = val;
}

KeyFrameData Player::GetKeyFrame()
{
    KeyFrameData key = KeyFrameData(m_transform, m_time, m_entityId, m_reportActionUsed);
    m_reportActionUsed = false;
	return key;
}

bool Player::StackRequested()
{
    bool ret = m_usedAction ? m_actionUsedTime == m_time : m_keyFrameTimer <= 0;
    if (ret)
    {
        m_keyFrameTimer = m_keyPeriod;
        m_keyFrameRequested = false;
    }
    return ret;
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