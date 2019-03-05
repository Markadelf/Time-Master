#include "stdafx.h"
#include "ServerSceneGraph.h"


ServerSceneGraph::ServerSceneGraph()
{
}


ServerSceneGraph::~ServerSceneGraph()
{
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
