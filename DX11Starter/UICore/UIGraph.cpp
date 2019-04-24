#include "UIGraph.h"
#include "AssetManager.h"

UIGraph::UIGraph() : UIGraph(15) {
}

UIGraph::UIGraph(int max)
{
	m_elements = new UIElement[max];
	m_maxElements = max;
	m_elementCount = 0;
	m_firstDirty = 0;
}

UIGraph::~UIGraph()
{
	delete[] m_elements;
}

int UIGraph::AddItem(UIElement& element)
{
	// Mark us as dirty because we changed the graph
	m_firstDirty = m_firstDirty < m_elementCount && m_firstDirty != -1 ? m_firstDirty : m_elementCount;
	m_elements[m_elementCount] = element;
	return m_elementCount++;
}

void UIGraph::Draw(DirectX::SpriteBatch& sb)
{
	for (size_t i = 0; i < m_elementCount; i++)
	{
		UIElement ele = m_elements[i];
		if (ele.m_textureHandle != -1)
		{
			DirectX::FXMVECTOR color = DirectX::XMLoadFloat4(&ele.m_color);
			sb.Draw(*AssetManager::get().GetTexturePointer(ele.m_textureHandle), ele.m_rect, color);
		}
	}
}

UIElement& UIGraph::GetElement(int handle)
{
	// Assume the element will be modified
	m_firstDirty = m_firstDirty < handle && m_firstDirty != -1 ? m_firstDirty : handle;
	return m_elements[handle];
}

void UIGraph::Recalculate(int width, int height)
{
	for (; m_firstDirty < m_elementCount; m_firstDirty++)
	{
		UIElement &element = m_elements[m_firstDirty];
		if (element.m_transform.m_parent == -1 || element.m_transform.m_parent > m_firstDirty)
		{
			// There isn't a parent, just calculate the rectangle
			element.m_rect.left = (long)((element.m_transform.m_anchor.GetX() - element.m_transform.m_pivot.GetX() / 2) * width);
			element.m_rect.top = (long)((element.m_transform.m_anchor.GetY() - element.m_transform.m_pivot.GetY() / 2) * height);
			element.m_rect.right = element.m_rect.left + (long)(element.m_transform.m_size.GetX() * width);
			element.m_rect.bottom = element.m_rect.top + (long)(element.m_transform.m_size.GetY() * height);
		}
		else
		{
			// There is a parent, treat it as the screen
			RECT parent = m_elements[element.m_transform.m_parent].m_rect;
			int pWidth = parent.right - parent.left;
			int pHeight = parent.bottom - parent.top;
			element.m_rect.left = parent.left + (long)((element.m_transform.m_anchor.GetX() - element.m_transform.m_pivot.GetX() / 2) * pWidth);
			element.m_rect.top = parent.top + (long)((element.m_transform.m_anchor.GetY() - element.m_transform.m_pivot.GetY() / 2) * pHeight);
			element.m_rect.right = element.m_rect.left + (long)(element.m_transform.m_size.GetX() * pWidth);
			element.m_rect.bottom = element.m_rect.top + (long)(element.m_transform.m_size.GetY() * pHeight);
		}
	}
	m_firstDirty = -1;
}

