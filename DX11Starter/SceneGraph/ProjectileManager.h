#pragma once
#include "Projectile.h"

struct ProjectileHandle
{
	int m_playerId;
	int m_bulletId;
	
	ProjectileHandle()
	{
		m_playerId = -1;
		m_bulletId = -1;
	}
	
	ProjectileHandle(int playerId, int bulletId) 
	{
		m_playerId = playerId;
		m_bulletId = bulletId;
	}
};

class ProjectileManager
{
	Projectile* m_projectiles;
	int* m_indices;
	int m_teams;
	int m_teamSize;
	int m_maxBullets;

public:
	ProjectileManager();
	ProjectileManager(int teams, int teamSize, int bulletsPerPlayer);
	~ProjectileManager();

	// Logic for adding and removing projectiles
	ProjectileHandle AddBullet(int playerId, const Projectile& bullet);
	void PopBullets(ProjectileHandle handle);

};

