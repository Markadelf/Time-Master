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
#include "serviceLocator.h"

// CLASSES //////////////////////////////////////////////////////////////////////////////
namespace input
{
	// enumerate all game commands
	enum Events : int;																// enumeration of all standard application events
	enum GameCommands : int;														// enumeration of all possible game commands
	enum KeyState { StillReleased, JustPressed, StillPressed, JustReleased };		// enumeration of all possible key states

	// structure to combine key codes and key states
	// each game command can have several bindings (chord), i.e. toggle show FPS = 'shift'+'control'+'FPS'
	struct BindInfo
	{
	private:
		unsigned int keyCode;		// the actual key code
		KeyState keyState;			// the state the above specified key has to be in for the "binding" to become active

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
		std::wstring name;						// human readable name
		std::vector<BindInfo> chord;			// the chord mapped to this command, i.e. "shift"+"control"+"F"

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
		std::array<BYTE, 256> keyboardStateCurrent;					// the state of the keyboard in the current frame 
		std::array<BYTE, 256> keyboardStatePrevious;				// the state of the keyboard in the previous frame

		void getKeyboardState();									// gets the keyboard state, uses GetAsyncKeyState to read the state of all 256 keys
		const KeyState getKeyboardKeyState(const unsigned int keyCode) const;// gets the state of the specified key, depending on its state in the previous and the current frame

		inline const bool isPressed(int keyCode) const { return (GetAsyncKeyState(keyCode) & 0x8000) ? 1 : 0; };	// returns true iff the key is down

		// polling
		void update();												// update the active key map
	protected:
		std::unordered_map<GameCommands, GameCommand*> keyMap;		// list of all possible game commands mapped to the appropriate command structure

		// constructor and destructor
		InputManager();
		~InputManager();

		// initialization
		virtual void setDefaultKeyMap() = 0;						// set up default controls

		// getters and setters
		util::Expected<std::wstring> getKeyName(const unsigned int keyCode);// retrieves the name of a virtual key code

	public:
		std::unordered_map<GameCommands, GameCommand*> activeKeyMap;// list of all active key maps; game acts on each command in this list

		// acquire input
		void acquireInput();
	};
}