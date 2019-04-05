#pragma once
// NOT UNNECESSARY
#include "Winsock2.h"
// NOT UNNECESSARY

#include <SpriteBatch.h>
#include "UIStructs.h"

class UIGraph {
	// Transform hierarchy
	UITransform* m_transforms;
	int m_maxTransformCount;
	int m_transformCount;

	// Objects
	UIText* m_texts;
	UIImage* m_images;
	UIButton* m_buttons;

	int m_maxObjectsPerType;
	int m_textCount;
	int m_imageCount;
	int m_buttonCount;

public:

	UIGraph();
	~UIGraph();

	int AddItem(int parent, Vector2 anchor, UIText text);
	int AddItem(int parent, Vector2 anchor, Vector2 size, UIImage image);
	int AddItem(int parent, Vector2 anchor, Vector2 size, UIButton button);

	void Draw(DirectX::SpriteBatch sb, float baseDepth, float depthPortion);

};