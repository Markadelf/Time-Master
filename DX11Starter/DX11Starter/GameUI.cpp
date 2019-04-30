#include "GameUI.h"
#include "Game.h"
#include "AssetManager.h"

GameUI::GameUI()
{
}

GameUI::~GameUI()
{
}

GameUI& GameUI::Get()
{
    static GameUI singleton;
    return singleton;
}

void GameUI::GivePointers(ID3D11Device* device, ID3D11DeviceContext* context)
{
    this->device = device;
    this->context = context;
}

void GameUI::InitializeUI()
{
    InitializeMainMenu();
    InitializeInGameUI();
}

int GameUI::InitializeMainMenu()
{
    AssetManager::get().LoadTexture(L"Textures/Background.png", device, context);
    int backGroundHandle = AssetManager::get().GetTextureHandle("Textures/Background.png");
    AssetManager::get().LoadTexture(L"Textures/PlayButton.png", device, context);
    int playButtonHandle = AssetManager::get().GetTextureHandle("Textures/PlayButton.png");
    AssetManager::get().LoadTexture(L"Textures/OptionsButton.png", device, context);
    int optionsButtonHandle = AssetManager::get().GetTextureHandle("Textures/OptionsButton.png");
    AssetManager::get().LoadTexture(L"Textures/CreditsButton.png", device, context);
    int creditsButtonHandle = AssetManager::get().GetTextureHandle("Textures/CreditsButton.png");

    mainMenuHandle = UIManager::get().MakeGraph();
    UIGraph& mainMenu = UIManager::get().GetGraph(mainMenuHandle);
    UIManager::get().SetGraphActiveInFront(mainMenuHandle);

    UIElement backGround;
    backGround.m_transform.m_size = Vector2(1, 1);
    backGround.m_textureHandle = backGroundHandle;

    UIElement bar;
    bar.m_transform.m_size = Vector2(1, .1f);
    bar.m_transform.m_anchor = Vector2(0, .9f);

    UIElement playButton;
    playButton.m_textureHandle = playButtonHandle;
    playButton.m_transform.m_size = Vector2(1/3.f, 1);
    playButton.m_eventBinding = UIManager::get().Bind(StartGame);


    UIElement optionsButton;
    optionsButton.m_textureHandle = optionsButtonHandle;
    optionsButton.m_transform.m_size = Vector2(1 / 3.f, 1);
    optionsButton.m_transform.m_anchor = Vector2(1 / 3.f, 0);


    // Second graph
    creditsHandle = InitializeCreditsUI();
    UIElement creditsButton;
    // Add button for graph 2 to graph one
    creditsButton.m_textureHandle = creditsButtonHandle;
    creditsButton.m_transform.m_size = Vector2(1 / 3.f, 1);
    creditsButton.m_transform.m_anchor = Vector2(2 / 3.f, 0);
    creditsButton.m_eventBinding = UIManager::get().Bind(UIManager::OpenUIFront);
    creditsButton.m_eventArg = creditsHandle;

    // Add items to graph
    mainMenu.AddItem(backGround);
    int barHandle = mainMenu.AddItem(bar);
    playButton.m_transform.m_parent = barHandle;
    optionsButton.m_transform.m_parent = barHandle;
    creditsButton.m_transform.m_parent = barHandle;
    mainMenu.AddItem(playButton);
    mainMenu.AddItem(optionsButton);
    mainMenu.AddItem(creditsButton);

    return mainMenuHandle;
}

int GameUI::InitializeInGameUI()
{
    ingameUIHandle = UIManager::get().MakeGraph();
    UIGraph& inGameUI = UIManager::get().GetGraph(ingameUIHandle);

    UIElement element;
    element.m_transform.m_size = Vector2(.08f, .1f);
    element.m_transform.m_anchor = Vector2(0, 0);
    element.m_transform.m_pivot = Vector2(0, 0);
    element.m_color = DirectX::XMFLOAT4(0, 1, 0, 1);
    element.m_transform.m_parent = inGameUI.AddItem(element);

    element.m_textureHandle = 1;
    element.m_transform.m_size = Vector2(.5f, .5f);
    element.m_transform.m_anchor = Vector2(.5f, .5f);
    element.m_transform.m_pivot = Vector2(.5f, .5f);
    element.m_color = DirectX::XMFLOAT4(1, 0, 0, 1);
    element.m_eventArg = mainMenuHandle;
    element.m_eventBinding = UIManager::get().Bind(ExitToMenu);

    element.m_transform.m_parent = inGameUI.AddItem(element);

    return creditsHandle;
}

int GameUI::InitializeCreditsUI()
{
    creditsHandle = UIManager::get().MakeGraph();
    UIGraph& credits = UIManager::get().GetGraph(creditsHandle);

    UIElement root;
    root.m_transform.m_size = Vector2(.5f, .5f);
    root.m_transform.m_anchor = Vector2(.5f, .5f);
    root.m_transform.m_pivot = Vector2(.5f, .5f);
    root.m_color = DirectX::XMFLOAT4(0, 1, 0, 1);
    root.m_textureHandle = 0;

    UIElement element;
    element.m_transform.m_parent = credits.AddItem(root);

    element.m_textureHandle = 0;
    element.m_transform.m_size = Vector2(.08f, .1f);
    element.m_transform.m_anchor = Vector2(0, 0);
    element.m_transform.m_pivot = Vector2(0, 0);
    element.m_color = DirectX::XMFLOAT4(0, 1, 0, 1);
    element.m_transform.m_parent = credits.AddItem(element);

    element.m_textureHandle = 1;
    element.m_transform.m_size = Vector2(.5f, .5f);
    element.m_transform.m_anchor = Vector2(.5f, .5f);
    element.m_transform.m_pivot = Vector2(.5f, .5f);
    element.m_color = DirectX::XMFLOAT4(1, 0, 0, 1);
    element.m_eventBinding = UIManager::get().Bind(UIManager::CloseUI);

    element.m_transform.m_parent = credits.AddItem(element);

    return creditsHandle;
}

void GameUI::ExitToMenu(int graph, int args)
{
    Game::UpdateGameState(GameState::MenuOnly);
    UIManager::MoveToUI(graph, args);
}

void GameUI::StartGame(int graph, int args)
{
    UIManager::MoveToUI(graph, Get().ingameUIHandle);
    Game::UpdateGameState(GameState::InGame);
}
