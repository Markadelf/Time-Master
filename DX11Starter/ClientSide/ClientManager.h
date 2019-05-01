#pragma once
#include "StaticObject.h"
#include "Player.h"
#include "SceneGraph.h"
#include "DrawGroup.h"
#include "ClientHelper.h"

// Client Manager is the interface used to update the scene graph and handle Controllers from the client side.
class ClientManager
{
	// Scene graph for rendering and/or Updating
	SceneGraph m_graph;

	// The controller for the player
	Player m_player;

	// Rendering information to be passed to the Renderer
	DrawGroup m_drawInfo;

	// Number of objects in the draw group we don't override each frame.
	int m_staticCount;

    // Nullptr if not communicating over the network
    ClientHelper* m_networkConnection;

public:
	ClientManager();
	~ClientManager();

	// Updates the player and communicates with the scenegraph
	void Update(float deltaTime);
	
	// Initializes the game state
	// TODO: Add apropriate parameters
	void Init(int entityId);

	// Accessor Functions
	Player& GetPlayer();
	SceneGraph& GetSceneGraph();
	// Rebuilds the Draw group when called
	DrawGroup& GetDrawGroup();
	
    void SetNetworkPointer(ClientHelper* connection);

    void RecieveData(Buffer& data);

private:
	// Prepares the static objects from the scenegraph
	void PrepDrawGroupStatics();

	// Prepares the draw group for each frame
	// Currently only partitions based on time
	void PrepDrawGroup();

	// Helper function that builds a draw item from a transform and a handle
	void ItemFromTransHandle(DrawItem& item, Transform trans, HandleObject handle);
};
