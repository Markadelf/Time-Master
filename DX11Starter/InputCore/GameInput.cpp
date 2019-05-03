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


/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Bindings /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void GameInput::setDefaultKeyMap()
{
	keyMap.clear();
	
	keyMap[input::GameCommands::Quit] = new input::GameCommand(L"Quit", VK_ESCAPE, input::KeyState::JustPressed);	
	keyMap[input::GameCommands::Shoot] = new input::GameCommand(L"Shoot", VK_LBUTTON, input::KeyState::JustPressed);
	keyMap[input::GameCommands::PlayerMoveForward] = new input::GameCommand(L"PlayerMoveForward", 'W', input::KeyState::StillPressed);
	keyMap[input::GameCommands::PlayerMoveLeft] = new input::GameCommand(L"PlayerMoveLeft", 'A', input::KeyState::StillPressed);
	keyMap[input::GameCommands::PlayerMoveBack] = new input::GameCommand(L"PlayerMoveBack", 'S', input::KeyState::StillPressed);
	keyMap[input::GameCommands::PlayerMoveRight] = new input::GameCommand(L"PlayerMoveRight", 'D', input::KeyState::StillPressed);
	keyMap[input::GameCommands::ReverseTime] = new input::GameCommand(L"ReverseTime", 'R', input::KeyState::JustPressed);
	
	//Sample code structure for implmenting key combination binding
	//std::vector<input::BindInfo> bi;
	//bi.push_back(input::BindInfo(VK_SHIFT, input::KeyState::StillPressed));

}