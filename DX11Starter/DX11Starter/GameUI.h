#pragma once
#include "DXCore.h"

class GameUI
{
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    int mainMenuHandle;
    int creditsHandle;
    int ingameUIHandle;

    GameUI();
    ~GameUI();
public:
    static GameUI& Get();

    void GivePointers(ID3D11Device* device, ID3D11DeviceContext* context);

    void InitializeUI();
    

private:
    int InitializeMainMenu();

    int InitializeInGameUI();

    int InitializeCreditsUI();

    static void ExitToMenu(int graph, int args);

    static void StartGame(int graph, int args);
};

