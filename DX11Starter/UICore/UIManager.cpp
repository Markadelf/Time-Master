#include "UIManager.h"

UIManager::UIManager()
{
    m_graphs = new UIGraph[MAXGRAPHS];
    m_activeGraphs = new int[MAXGRAPHSACTIVE];

    m_graphCount = 0;
    m_activeCount = 0;

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
    memcpy(m_activeGraphs + 1, m_activeGraphs, m_activeCount * sizeof(int));

    // Insert at the back
    m_activeGraphs[0] = graphHandle;
    m_activeCount++;
    return true;
}

void UIManager::SetGraphInactive(int graphHandle)
{
    m_activeCount--;
    for (size_t i = 0; i < m_activeCount; i++)
    {
        if (m_activeGraphs[i] == graphHandle)
        {
            memcpy(&m_activeGraphs[i], &m_activeGraphs[i + 1], (m_activeCount - i) * sizeof(int));
            break;
        }
    }
}

void UIManager::SetScreenDimensions(int width, int height)
{
    for (size_t i = 0; i < m_graphCount; i++)
    {
        m_graphs[i].m_firstDirty = 0;
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
        if (graph.m_firstDirty != -1)
        {
            graph.Recalculate(m_width, m_height);
        }
        graph.Draw(*m_sb);
    }
    m_sb->End();
}

void UIManager::OnClick(int x, int y)
{
    if (m_activeCount <= 0)
    {
        return;
    }

    int handle = -1;
    int arg;

    // Only check the front graph
    UIGraph& graph = m_graphs[m_activeGraphs[m_activeCount - 1]];
    // Check if graphs need to be recalculated
    if (graph.m_firstDirty != -1)
    {
        graph.Recalculate(m_width, m_height);
    }
    handle = graph.GetClickEventHandle(x, y, arg);

    if (handle != -1)
    {
        m_eventBindings[handle](m_activeCount - 1, arg);
    }
}

int UIManager::Bind(UIEvent callback)
{
    for (int i = 0; i < m_eBindCount; i++)
    {
        if (m_eventBindings[i] == callback)
        {
            return i;
        }
    }
    m_eventBindings[m_eBindCount] = callback;
    return m_eBindCount++;
}

void UIManager::OpenUIFront(int graph, int arg)
{
    get().SetGraphActiveInFront(arg);
}

void UIManager::OpenUIBack(int graph, int arg)
{
    get().SetGraphActiveInBack(arg);
}

void UIManager::CloseUI(int graph, int arg)
{
    arg = arg == -1 ? graph : arg;
    get().SetGraphInactive(arg);
}

void UIManager::MoveToUI(int graph, int arg)
{
    get().m_activeCount = 0;
    get().SetGraphActiveInFront(arg);
}

void UIManager::SwapToUI(int graph, int arg)
{
    get().SetGraphInactive(graph);
    get().SetGraphActiveInFront(arg);
}
