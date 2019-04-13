#pragma once
#include "StaticObject.h"
#include "Player.h"
#include "SceneGraph.h"
#include "DrawGroup.h"

// Client Manager is the interfaced used to update the scene graph from the client side.
class ClientManager
{
	// Scene graph for rendering and/or Updating
	SceneGraph m_graph;

	Player m_player;

	DrawGroup m_drawInfo;

	int m_staticCount;

public:
	ClientManager();
	~ClientManager();

	void Update(float deltaTime);
	
	void Init();

	Player& GetPlayer();

	SceneGraph& GetSceneGraph();

	DrawGroup& GetDrawGroup();
	
private:
	void PrepDrawGroupStatics();

	void PrepDrawGroup();

	void ItemFromTransHandle(DrawItem& item, Transform trans, HandleObject handle);
};
