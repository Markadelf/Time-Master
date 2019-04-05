#pragma once

#include "UIGraph.h"

// The UIManager class is the main interface through which the engine accesses the Menu System
// The UIManager Handles all of the UIGraphs and communicates any input across them
class UIManager
{
	UIGraph* m_graphs;
	int* m_activeGraphs;

	// The max number of graphs we can store at once
	int m_maxGraphs;
	// The max number of graphs active in a frame
	int m_maxGraphsActive;
	
	// Number of graphs loaded
	int m_graphCount;
	// Number of graphs currently being drawn
	int m_activeCount;

	// This is the graph that gets any keyboard input sent to it
	int m_inputGraph;

public:
	UIManager();
	~UIManager();

	int LoadGraph();

	void SetGraphActive(int graphHandle, bool addToBack = false);
	void SetGraphInactive(int graphHandle);

	// TODO
	void PassInput();
	void Render(DirectX::SpriteBatch sb);
};

