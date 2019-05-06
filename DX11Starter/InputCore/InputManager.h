#pragma once
//Windows & COM
#include <Windows.h>

//Data Structures
#include <unordered_map>
#include <array>
#include <vector>

//Strings
#include <iostream>

// Helper Utility
#include "expected.h"


// CLASSES //////////////////////////////////////////////////////////////////////////////
namespace input
{
	// enumerate all game commands

	// enumeration of all standard application events
	enum Events : int;			
	// enumeration of all possible game commands
	enum GameCommands : int;					
	// enumeration of all possible key states
	enum KeyState { StillReleased, JustPressed, StillPressed, JustReleased };		

	// structure to combine key codes and key states
	// each game command can have several bindings (chord), i.e. toggle show FPS = 'shift'+'control'+'FPS'
	struct BindInfo
	{
	private:
		// the actual key code
		unsigned int keyCode;
		// the state the above specified key has to be in for the "binding" to become active
		KeyState keyState;			

	public:
		// constructors and destructor
		BindInfo();
		BindInfo(const unsigned int keyCode, const KeyState keyState);
		~BindInfo() {};

		friend struct GameCommand;
		friend class InputManager;
	};

	// structure to map a single game command to a chord of key strokes (see above)
	struct GameCommand
	{
	private:
		// human readable name
		std::wstring name;	
		// the chord mapped to this command, example. "shift"+"control"+"F"
		std::vector<BindInfo> chord;			

	public:
		// constructors and destructor
		GameCommand();
		GameCommand(const std::wstring& name, const unsigned int keyCode, const KeyState keyState);
		GameCommand(const std::wstring& name, const BindInfo& bi);
		GameCommand(const std::wstring& name, const std::vector<BindInfo>& chord);
		~GameCommand() {};

		friend class InputManager;
	};

	// the main input manager class
	class InputManager
	{
	private:
		/////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////// KEYBOARD /////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////
		// the state of the keyboard in the current frame 
		std::array<BYTE, 256> keyboardStateCurrent;			
		// the state of the keyboard in the previous frame
		std::array<BYTE, 256> keyboardStatePrevious;				
		// gets the keyboard state, uses GetAsyncKeyState to read the state of all 256 keys
		void getKeyboardState();									
		// gets the state of the specified key, depending on its state in the previous and the current frame
		const KeyState getKeyboardKeyState(const unsigned int keyCode) const;
		// returns true iff the key is down
		inline const bool isPressed(int keyCode) const { return (GetAsyncKeyState(keyCode) & 0x8000) ? 1 : 0; };	

		// polling
		// update the active key map
		void update();												
	protected:
		// list of all possible game commands mapped to the appropriate command structure
		std::unordered_map<GameCommands, GameCommand*> keyMap;		

		// constructor and destructor
		InputManager();
		~InputManager();

		// initialization
		// set up default controls
		virtual void setDefaultKeyMap() = 0;						

		// getters and setters
		// retrieves the name of a virtual key code
		util::Expected<std::wstring> getKeyName(const unsigned int keyCode);

	public:
		// list of all active key maps; game acts on each command in this list
		std::unordered_map<GameCommands, GameCommand*> activeKeyMap;

		// acquire input
		void acquireInput();
	};
}