#include "GameInput.h"

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Input Handler ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
GameInput::GameInput()
{
	setDefaultKeyMap();
}

GameInput::~GameInput()
{
}

void GameInput::setDefaultKeyMap()
{
	keyMap.clear();
	std::vector<input::BindInfo> bi;
	bi.push_back(input::BindInfo(VK_SHIFT, input::KeyState::StillPressed));
	bi.push_back(input::BindInfo(VK_CONTROL, input::KeyState::StillPressed));
	bi.push_back(input::BindInfo('F', input::KeyState::JustPressed));
	keyMap[input::GameCommands::Quit] = new input::GameCommand(L"Quit", VK_ESCAPE, input::KeyState::JustPressed);
	keyMap[input::GameCommands::showFPS] = new input::GameCommand(L"showFPS", bi);
}