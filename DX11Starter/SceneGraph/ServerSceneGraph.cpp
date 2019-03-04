#include "stdafx.h"
#include "ServerSceneGraph.h"


ServerSceneGraph::ServerSceneGraph()
{
}


ServerSceneGraph::~ServerSceneGraph()
{
}

void ServerSceneGraph::StackPhantom(PhantomImageData phantom)
{
	int playerId = phantom.GetPlayerId();
	// TODO: Check for collisions
	m_players[playerId].TrackMovement(phantom.GetTransform(), phantom.GetTimeStamp());
	if (phantom.GetShot()) {
		// TODO: Check for collisions
		// TODO: Fix handles
		Transform transform = m_players[playerId].Head().GetTransform(phantom.GetShotTime());
		const float BULLETRANGE = 10;
		const TimeStamp BULLETPERIOD = 10;
		Vector2 finalPos = Vector2(BULLETRANGE, 0).Rotate(transform.GetRot());
		TimeInstableTransform traj = TimeInstableTransform(transform, Transform(finalPos, transform.GetRot()), phantom.GetShotTime(), phantom.GetShotTime() + BULLETPERIOD, false);
		Projectile spawn = Projectile(traj, HandleObject());
		m_projectileManager.AddBullet(playerId, spawn);
		m_players[playerId].TrackBulletShot(phantom.GetShotTime());
	}

}
