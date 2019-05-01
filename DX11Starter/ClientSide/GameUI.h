#pragma once
#include "DXCore.h"

class GameUI
{
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    int mainMenuHandle;
    int creditsHandle;
    int controlsHandle;
    int ingameUIHandle;
    int resultScreenHandle;
	int waitingHandle;

    int resultImages[4];
    int resultEle;

    int barEle;
    float barScale;
    
    int deathEle;


    GameUI();
    ~GameUI();
public:
    static GameUI& Get();

    // Passes pointers for loading textures
    void GivePointers(ID3D11Device* device, ID3D11DeviceContext* context);

    // Initializes the ui
    void InitializeUI();

    // Exits the game and moves to the results screen
    void ExitToResults(int resultCode);

    void UpdateGameUI(bool dead, float time);

	void WaitForNetwork();

	void DisplayHUD();

private:
    void InitializeMainMenu();

    void InitializeInGameUI();

    void InitializeCreditsUI();

    void InitializeControlsUI();

	void InitializeResultScreen();

	void InitializeWaitScreen();

    static void ExitToMenu(int graph, int args);

    static void JoinGame(int graph, int args);
};

