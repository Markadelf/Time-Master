#include "UIManager.h"

UIManager::UIManager()
{
	m_graphs = new UIGraph[MAXGRAPHS];
	m_activeGraphs = new int[MAXGRAPHSACTIVE];

	m_graphCount = 0;
	m_activeCount = 0;

	m_inputGraph = -1;

	m_sb = nullptr;
}

UIManager::~UIManager()
{
	if (m_graphs != nullptr)
	{
		delete[] m_graphs;
		m_graphs = nullptr;
	}
	if (m_activeGraphs != nullptr)
	{
		delete[] m_activeGraphs;
		m_activeGraphs = nullptr;
	}
	if (m_sb != nullptr)
	{
		delete m_sb;
		m_sb = nullptr;
	}
}

UIManager& UIManager::get()
{
	static UIManager singleton;
	return singleton;
}

void UIManager::SetContext(ID3D11DeviceContext* ctx)
{
	if (m_sb != nullptr)
	{
		delete m_sb;
		m_sb = nullptr;
	}
	m_sb = new DirectX::SpriteBatch(ctx);
}

int UIManager::MakeGraph()
{
	if (m_graphCount >= MAXGRAPHS)
	{
		return -1;
	}
	return m_graphCount++;
}

UIGraph& UIManager::GetGraph(int index)
{
	return m_graphs[index];
}

bool UIManager::SetGraphActiveInFront(int graphHandle)
{
	if (m_activeCount >= MAXGRAPHSACTIVE)
	{
		return false;
	}

	m_activeGraphs[m_activeCount++] = graphHandle;
	return true;
}

bool UIManager::SetGraphActiveInBack(int graphHandle)
{
	if (m_activeCount >= MAXGRAPHSACTIVE)
	{
		return false;
	}

	// Move the rest of the list forward
	memcpy(m_activeGraphs + 1, m_activeGraphs, m_activeCount);

	// Insert at the back
	m_activeGraphs[0] = graphHandle;
	m_activeCount++;
	return true;
}

void UIManager::SetGraphInactive(int graphHandle)
{
}

void UIManager::SetScreenDimensions(int width, int height)
{
	for (size_t i = 0; i < m_graphCount; i++)
	{
		m_graphs[i].m_isDirty = true;
	}
	m_width = width;
	m_height = height;
}

void UIManager::Render()
{
	m_sb->Begin();
	for (size_t i = 0; i < m_activeCount; i++)
	{
		UIGraph& graph = m_graphs[m_activeGraphs[i]];
		// Check if graphs need to be recalculated
		if (graph.m_isDirty)
		{
			graph.Recalculate(m_width, m_height);
		}
		graph.Draw(*m_sb);
	}
	m_sb->End();
}
