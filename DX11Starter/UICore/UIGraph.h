#pragma once
// NOT UNNECESSARY
#include "Winsock2.h"
// NOT UNNECESSARY

#include <SpriteBatch.h>
#include "UIStructs.h"

// UI Graph is a class that holds the UI data for one menu or HUD
// Currently does not support removing items at runtime
// To get around this, consider making the texture handle -1.
class UIGraph {
	// UI elements
	UIElement* m_elements;

	int m_maxElements;
	int m_elementCount;

	// Tracks if we need to 
	bool m_isDirty;

public:

	UIGraph();
	UIGraph(int max);
	~UIGraph();

	// Adds an element to the UI
	int AddItem(UIElement& element);

	// Draws the graph via the spritebatch
	void Draw(DirectX::SpriteBatch& sb);

	// Returns a reference to the requested item
	UIElement& GetElement(int handle);

private:
	// Used to rebake the positions of elements
	void Recalculate(int width, int height);

	// This allows UIManager to access m_isDirty
#include "UIManager.h"
	friend class UIManager;
};