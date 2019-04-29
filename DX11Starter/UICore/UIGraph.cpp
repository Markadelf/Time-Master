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

int UIGraph::GetClickEventHandle(int x, int y, int& arg)
{
    int handle = -1;
    // Loop from front to back
    for (int i = m_elementCount - 1; i >= 0 && handle == -1; i--)
    {
        RECT& rect = m_elements[i].m_rect;
        if (rect.left < x && rect.right > x && rect.top < y && rect.bottom > y && m_elements[i].m_eventBinding != -1)
        {
            arg = m_elements[i].m_eventArg;
            return m_elements[i].m_eventBinding;
        }
    }
    return -1;
}

void UIGraph::Recalculate(int width, int height)
{
    RECT screen;
    screen.left = 0;
    screen.top = 0;
    screen.right = width;
    screen.bottom = height;
	for (; m_firstDirty < m_elementCount; m_firstDirty++)
	{
		UIElement &element = m_elements[m_firstDirty];
        UITransform& trans = element.m_transform;

        if (trans.m_parent == -1 || trans.m_parent > m_firstDirty)
		{
			// There isn't a parent, just calculate the rectangle
            element.m_rect = CalculateRect(trans, screen);
		}
		else
		{
            // There is a parent, treat it as the screen
            element.m_rect = CalculateRect(trans, m_elements[element.m_transform.m_parent].m_rect);
		}
	}
	m_firstDirty = -1;
}

RECT UIGraph::CalculateRect(UITransform& transform, RECT& parent)
{
    int width = parent.right - parent.left;
    int height = parent.bottom - parent.top;
    RECT ret;

    ret.left = parent.left + (long)((transform.m_anchor.m_x - transform.m_pivot.m_x * transform.m_size.m_x) * width);
    ret.top = parent.top + (long)((transform.m_anchor.m_y - transform.m_pivot.m_y * transform.m_size.m_y) * height);
    ret.right = ret.left + (long)(transform.m_size.m_x * width);
    ret.bottom = ret.top + (long)(transform.m_size.m_y * height);
    return ret;
}

