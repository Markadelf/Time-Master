#pragma once

#include "UIGraph.h"

// The UIManager class is the main interface through which the engine accesses the Menu System
// The UIManager Handles all of the UIGraphs and communicates any input across them
class UIManager
{
	static UIManager* s_instance;

	// UIGraphs
	UIGraph* m_graphs;
	int* m_activeGraphs;
	DirectX::SpriteBatch* m_sb;

	// The max number of graphs we can store at once
	static const int MAXGRAPHS = 10;
	// The max number of graphs active in a frame
	static const int MAXGRAPHSACTIVE = 5;
	
	// Number of graphs loaded
	int m_graphCount;
	// Number of graphs currently being drawn
	int m_activeCount;

	// This is the graph that gets any keyboard input sent to it
	int m_inputGraph;

	// Screen Dimensions
	int m_width;
	int m_height;

	UIManager();

	~UIManager();

public:

	// Get function for singleton
	static UIManager& get();

	// Initializes the Spritebatch by passing the context in
	void SetContext(ID3D11DeviceContext* ctx);

	// Creates a new empty graph and returns a handle
	int MakeGraph();
	// Returns a reference to a graph
	UIGraph& GetGraph(int index);

	// Adds a graph to the active set
	bool SetGraphActiveInFront(int graphHandle);
	bool SetGraphActiveInBack(int graphHandle);
	// Removes a graph from the active set
	void SetGraphInactive(int graphHandle);

	// Uses the screen dimensions to properly resize ui elements
	void SetScreenDimensions(int width, int height);

	// Renders to the screen
	void Render();

	// TODO: ADD Controls and event system for UI
};

