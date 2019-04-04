#pragma once
#include <string>
#include "Vector2.h"

struct UITransform
{
	Vector2 m_anchor;
	Vector2 m_size;

	int m_parent;

	int m_text = -1;
	int m_image = -1;
	int m_button = -1;
};

struct UIText {
	std::string m_text;
};

struct UIImage {
	int m_textureHandle;
};

struct UIButton {
	int m_binding;
};



