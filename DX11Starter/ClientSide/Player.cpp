#include "Player.h"
#include "ColliderManager.h"

Player::Player()
{
	m_reversed = false;
	// initialize the input manager
	PlayerInput = new GameInput();
}


Player::~Player()
{
	delete PlayerInput;
}

void Player::Initialize(const Transform& startingPos, float initialTime, HandleObject handle)
{
	m_transform = startingPos;
	m_time = initialTime;
	m_handle = handle;
}

void Player::acquireInput(float deltaTime)
{
	PlayerInput->acquireInput();
	Vector2 vel;
		// act on user input
		for (auto x : PlayerInput->activeKeyMap)
		{
			switch (x.first)
			{
			//case input::GameCommands::Quit:
			//	m_renderer.Quit();
			//	break;

			//case input::GameCommands::showFPS:
			//	printf("TEST FPS");
			//	break;

			case input::GameCommands::Shoot:
				printf("SHOOTING");
				m_lastTimeShot = m_time;
				m_shot = true;
				break;

			case input::GameCommands::PlayerMoveForward:
				printf("W");
				vel = vel + Vector2(0, 1);
				break;

			case input::GameCommands::PlayerMoveBack:
				printf("S");
				vel = vel + Vector2(0, -1);
				break;
			case input::GameCommands::PlayerMoveLeft:
				printf("A");
				vel = vel + Vector2(-1, 0);
				break;
			case input::GameCommands::PlayerMoveRight:
				printf("D");
				vel = vel + Vector2(1, 0);
				break;
			case input::GameCommands::ReverseTime:
				m_reversed = !m_reversed;
				break;
							
			}

			vel = vel.Rotate(-m_transform.GetRot());
			m_transform.SetPos(m_transform.GetPos() + vel * deltaTime);
		}	
}

void Player::Update(float deltaTime)
{
	m_time += deltaTime * (m_reversed ? -1 : 1);

	UpdatePosition(deltaTime);

	acquireInput(deltaTime);
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
	acquireInput(deltaTime);
}