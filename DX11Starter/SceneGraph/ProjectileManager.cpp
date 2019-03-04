#include "stdafx.h"
#include "ProjectileManager.h"


ProjectileManager::ProjectileManager() : ProjectileManager(1, 1, 64)
{
}

ProjectileManager::ProjectileManager(int teams, int teamSize, int bulletsPerPlayer)
{
	m_teams = teams;
	m_teamSize = teamSize;
	m_maxBullets = bulletsPerPlayer;
	m_projectiles = new Projectile[teams * teamSize * bulletsPerPlayer];
	m_indices = new int[teams * teamSize];
	for (size_t i = 0; i < teams * teamSize; i++)
	{
		m_indices[i] = 0;
	}
}


ProjectileManager::~ProjectileManager()
{
	delete[] m_projectiles;
	delete[] m_indices;
}

ProjectileHandle ProjectileManager::AddBullet(int playerId, const Projectile& bullet)
{
	if (m_indices[playerId] + 1 < m_maxBullets)
	{
		m_projectiles[playerId * m_maxBullets + m_indices[playerId]] = bullet;
		return ProjectileHandle(playerId, m_indices[playerId]++);
	}
	return ProjectileHandle();
}

void ProjectileManager::PopBullets(ProjectileHandle handle)
{
	m_indices[handle.m_playerId] = handle.m_bulletId;
}
