#include "ServerSceneGraph.h"


ServerSceneGraph::ServerSceneGraph()
{
	m_players = nullptr;
	m_statics = nullptr;
}

ServerSceneGraph::ServerSceneGraph(int teams, int playersPerTeam, int maxBullets, int maxColliders, StaticObject* staticObjs, int staticobjectCount)
{
	m_playerCount = teams * playersPerTeam;
	m_colliderManager = ColliderManager(maxColliders, maxColliders);
	m_projectileManager = ProjectileManager(teams, playersPerTeam, maxBullets);

	m_players = new PlayerObject[teams * playersPerTeam];

	// Static Object Buffer
	m_statics = new StaticObject[staticobjectCount];
	memcpy(m_statics, staticObjs, staticobjectCount);

	m_playerCount = teams * playersPerTeam;
	// Number of StaticObjects
	int m_staticObjectCount;

	// Time limit of match in real time
	m_matchTimeRealTime = 300;
	// Time constarints of match space
	m_matchTimeRangeGameTime = 60;

	m_matchTimer = 0;
}


ServerSceneGraph::~ServerSceneGraph()
{
	if (m_players)
	{
		delete[] m_players;
	}
	if (m_statics) {
		delete[] m_statics;
	}
}

void ServerSceneGraph::StackKeyFrame(PlayerKeyFrameData keyFrame)
{
	int playerId = keyFrame.GetPlayerId();
	// TODO: Check for collisions
	m_players[playerId].StackKeyFrame(keyFrame);
	if (keyFrame.GetShot()) {
		// TODO: Check for collisions
		// TODO: Fix handles
		// TODO: MAKE DIFFERENT BULLET TYPES POSSIBLE
		Transform transform = m_players[playerId].Head().GetTransform().GetTransform(keyFrame.GetShotTime());
		const float BULLETRANGE = 10;
		const TimeStamp BULLETPERIOD = 10;
		Vector2 finalPos = Vector2(BULLETRANGE, 0).Rotate(transform.GetRot());
		TimeInstableTransform traj = TimeInstableTransform(transform, Transform(finalPos, transform.GetRot()), keyFrame.GetShotTime(), keyFrame.GetShotTime() + BULLETPERIOD, false);
		Projectile spawn = Projectile(traj, HandleObject());
		m_projectileManager.AddBullet(playerId, spawn);
	}

}
