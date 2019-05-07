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
    InitializeResultScreen();
	InitializeWaitScreen();
    barScale = 100;
}

void GameUI::ExitToResults(int resultCode)
{
    UIManager::get().GetGraph(resultScreenHandle).GetElement(resultEle).m_textureHandle = resultImages[resultCode];
    ExitToMenu(0, resultScreenHandle);
}

void GameUI::UpdateGameUI(bool dead, float time)
{
    float percentage = .5f + time / barScale;
    percentage = percentage < 1 ? (percentage > 0 ? percentage : 0) : 1;
    UIManager::get().GetGraph(ingameUIHandle).GetElement(barEle).m_transform.m_anchor.m_x = percentage;

    UIManager::get().GetGraph(ingameUIHandle).GetElement(deathEle).m_color = dead ? DirectX::XMFLOAT4(1, 1, 1, 1) : DirectX::XMFLOAT4(0, 0, 0, 0);
}

void GameUI::UpdateIconColor(DirectX::XMFLOAT4 col)
{
    UIManager::get().GetGraph(ingameUIHandle).GetElement(barEle).m_color = col;
}

void GameUI::WaitForNetwork()
{
	UIManager::MoveToUI(0, waitingHandle);
}

void GameUI::DisplayHUD()
{
	UIManager::MoveToUI(0, Get().ingameUIHandle);
}

void GameUI::InitializeMainMenu()
{
    AssetManager::get().LoadTexture(L"Textures/Background.png", device, context);
    int backGroundHandle = AssetManager::get().GetTextureHandle("Textures/Background.png");
    AssetManager::get().LoadTexture(L"Textures/PlayButton.png", device, context);
    int playButtonHandle = AssetManager::get().GetTextureHandle("Textures/PlayButton.png");
    AssetManager::get().LoadTexture(L"Textures/PlayOffline.png", device, context);
    int playOffline = AssetManager::get().GetTextureHandle("Textures/PlayOffline.png");
    AssetManager::get().LoadTexture(L"Textures/ControlsButton.png", device, context);
    int optionsButtonHandle = AssetManager::get().GetTextureHandle("Textures/ControlsButton.png");
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
    playButton.m_transform.m_size = Vector2(.25f, 1);
    playButton.m_eventBinding = UIManager::get().Bind(JoinGame);

    UIElement playButton2 = playButton;
    playButton2.m_textureHandle = playOffline;
    playButton2.m_transform.m_anchor = Vector2(.25f, 0);
    playButton2.m_eventBinding = UIManager::get().Bind(StartGame);

    InitializeControlsUI();
    UIElement controlButton;
    controlButton.m_textureHandle = optionsButtonHandle;
    controlButton.m_transform.m_size = Vector2(.25f, 1);
    controlButton.m_transform.m_anchor = Vector2(.5f, 0);
    controlButton.m_eventBinding = UIManager::get().Bind(UIManager::OpenUIFront);
    controlButton.m_eventArg = controlsHandle;

    InitializeCreditsUI();
    UIElement creditsButton;
    // Add button for graph 2 to graph one
    creditsButton.m_textureHandle = creditsButtonHandle;
    creditsButton.m_transform.m_size = Vector2(.25f, 1);
    creditsButton.m_transform.m_anchor = Vector2(.75f, 0);
    creditsButton.m_eventBinding = UIManager::get().Bind(UIManager::OpenUIFront);
    creditsButton.m_eventArg = creditsHandle;

    // Add items to graph
    mainMenu.AddItem(backGround);
    int barHandle = mainMenu.AddItem(bar);
    playButton.m_transform.m_parent = barHandle;
    playButton2.m_transform.m_parent = barHandle;
    controlButton.m_transform.m_parent = barHandle;
    creditsButton.m_transform.m_parent = barHandle;
    mainMenu.AddItem(playButton);
    mainMenu.AddItem(playButton2);
    mainMenu.AddItem(controlButton);
    mainMenu.AddItem(creditsButton);
}

void GameUI::InitializeInGameUI()
{
    AssetManager::get().LoadTexture(L"Textures/Death.png", device, context);
    int deathImageHandle = AssetManager::get().GetTextureHandle("Textures/Death.png");

    AssetManager::get().LoadTexture(L"Textures/timeLine.png", device, context);
    int timeLineHandle = AssetManager::get().GetTextureHandle("Textures/timeLine.png");

    AssetManager::get().LoadTexture(L"Textures/icon.png", device, context);
    int iconHandle = AssetManager::get().GetTextureHandle("Textures/icon.png");

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

    // bar
    UIElement barHolder;
    barHolder.m_textureHandle = timeLineHandle;
    barHolder.m_transform.m_anchor = Vector2(.5f, .05f);
    barHolder.m_transform.m_pivot = Vector2(.5f, .5f);
    barHolder.m_transform.m_size = Vector2(.6f, .03f);
    barHolder.m_color = DirectX::XMFLOAT4(1, 1, 1, 1);
    
    UIElement bar;
    bar.m_textureHandle = iconHandle;
    bar.m_transform.m_parent = inGameUI.AddItem(barHolder);
    bar.m_transform.m_anchor.m_y = .5f;
    bar.m_transform.m_size = Vector2(.02f, 1.2f);
    bar.m_transform.m_pivot = Vector2(.75f, 1);
    bar.m_color = DirectX::XMFLOAT4(0, 1, 1, 1);

    barEle = inGameUI.AddItem(bar);

    UIElement death;
    death.m_textureHandle = deathImageHandle;
    death.m_transform.m_anchor = Vector2(.5f, .5f);
    death.m_transform.m_pivot = Vector2(.5f, .5f);
    death.m_transform.m_size = Vector2(.75f, .75f);

    deathEle = inGameUI.AddItem(death);

	UpdateGameUI(false, 0);
}

void GameUI::InitializeCreditsUI()
{
    AssetManager::get().LoadTexture(L"Textures/Credits.png", device, context);
    int creditsBackground = AssetManager::get().GetTextureHandle("Textures/Credits.png");

    creditsHandle = UIManager::get().MakeGraph();
    UIGraph& credits = UIManager::get().GetGraph(creditsHandle);

    UIElement root;
    root.m_transform.m_size = Vector2(.5f, .5f);
    root.m_transform.m_anchor = Vector2(.5f, .5f);
    root.m_transform.m_pivot = Vector2(.5f, .5f);
    root.m_textureHandle = creditsBackground;

    UIElement element;
    element.m_transform.m_size = Vector2(1, 1);
    element.m_eventBinding = UIManager::get().Bind(UIManager::CloseUI);

    credits.AddItem(element);
    credits.AddItem(root);
}

void GameUI::InitializeControlsUI()
{
    AssetManager::get().LoadTexture(L"Textures/Controls.png", device, context);
    int controlsBackground = AssetManager::get().GetTextureHandle("Textures/Controls.png");

    controlsHandle = UIManager::get().MakeGraph();
    UIGraph& controls = UIManager::get().GetGraph(controlsHandle);

    UIElement root;
    root.m_transform.m_size = Vector2(.5f, .5f);
    root.m_transform.m_anchor = Vector2(.5f, .5f);
    root.m_transform.m_pivot = Vector2(.5f, .5f);
    root.m_textureHandle = controlsBackground;

    UIElement element;
    element.m_transform.m_size = Vector2(1, 1);
    element.m_eventBinding = UIManager::get().Bind(UIManager::CloseUI);

    controls.AddItem(element);
    controls.AddItem(root);
}

void GameUI::InitializeResultScreen()
{
    AssetManager::get().LoadTexture(L"Textures/Victory.png", device, context);
    AssetManager::get().LoadTexture(L"Textures/Defeat.png", device, context);
    AssetManager::get().LoadTexture(L"Textures/Tie.png", device, context);
    AssetManager::get().LoadTexture(L"Textures/NetworkError.png", device, context);

    // TODO: Add different screens
    int controlsBackground = AssetManager::get().GetTextureHandle("Textures/Controls.png");

    // Victory
    resultImages[0] = AssetManager::get().GetTextureHandle("Textures/Victory.png");
    // Defeat
    resultImages[1] = AssetManager::get().GetTextureHandle("Textures/Defeat.png");
    // Tie
    resultImages[2] = AssetManager::get().GetTextureHandle("Textures/Tie.png");
    // Network Error
    resultImages[3] = AssetManager::get().GetTextureHandle("Textures/NetworkError.png");

    resultScreenHandle = UIManager::get().MakeGraph();
    UIGraph& results = UIManager::get().GetGraph(resultScreenHandle);

    UIElement root;
    root.m_transform.m_size = Vector2(.5f, .5f);
    root.m_transform.m_anchor = Vector2(.5f, .5f);
    root.m_transform.m_pivot = Vector2(.5f, .5f);
    root.m_textureHandle = controlsBackground;

    UIElement element;
    element.m_transform.m_size = Vector2(1, 1);
    element.m_eventArg = mainMenuHandle;
    element.m_eventBinding = UIManager::get().Bind(UIManager::MoveToUI);

    results.AddItem(element);
    resultEle = results.AddItem(root);
}

void GameUI::InitializeWaitScreen()
{
    int playButtonHandle = AssetManager::get().GetTextureHandle("Textures/PlayButton.png");
    AssetManager::get().LoadTexture(L"Textures/WaitingForNetwork.png", device, context);
	int waitBack = AssetManager::get().GetTextureHandle("Textures/WaitingForNetwork.png");

	waitingHandle = UIManager::get().MakeGraph();
	UIGraph& controls = UIManager::get().GetGraph(waitingHandle);

	UIElement root;
	root.m_transform.m_size = Vector2(.5f, .5f);
	root.m_transform.m_anchor = Vector2(.5f, .5f);
	root.m_transform.m_pivot = Vector2(.5f, .5f);
	root.m_textureHandle = waitBack;

    UIElement element;
    element.m_transform.m_size = Vector2(.08f, .1f);
    element.m_transform.m_anchor = Vector2(0, 0);
    element.m_transform.m_pivot = Vector2(0, 0);
    element.m_color = DirectX::XMFLOAT4(0, 1, 0, 1);
    element.m_transform.m_parent = controls.AddItem(element);

    element.m_textureHandle = 1;
    element.m_transform.m_size = Vector2(.5f, .5f);
    element.m_transform.m_anchor = Vector2(.5f, .5f);
    element.m_transform.m_pivot = Vector2(.5f, .5f);
    element.m_color = DirectX::XMFLOAT4(1, 0, 0, 1);
    element.m_eventArg = mainMenuHandle;
    element.m_eventBinding = UIManager::get().Bind(ExitToMenu);

    element.m_transform.m_parent = controls.AddItem(element);

    UIElement bar;
    bar.m_transform.m_size = Vector2(1, .1f);
    bar.m_transform.m_anchor = Vector2(0, .9f);

    UIElement playButton;
    playButton.m_textureHandle = playButtonHandle;
    playButton.m_transform.m_size = Vector2(1 / 3.f, 1);
    playButton.m_eventBinding = UIManager::get().Bind(StartGame);

    int barHandle = controls.AddItem(bar);
    playButton.m_transform.m_parent = barHandle;
    controls.AddItem(playButton);
	controls.AddItem(root);
}

void GameUI::ExitToMenu(int graph, int args)
{
    Game::UpdateGameState(GameState::MenuOnly);
    UIManager::MoveToUI(graph, args);
}

void GameUI::JoinGame(int graph, int args)
{
    UIManager::MoveToUI(graph, Get().waitingHandle);
    Game::UpdateGameState(GameState::WaitingForNetwork);
}

void GameUI::StartGame(int graph, int args)
{
    Game::StartGameOffline();
}
