#pragma once

#include "Renderer.h"
#include "GameInput.h"
#include "InputManager.h"
#include "ClientManager.h"

// Handles the game engine level a the highest level of abstraction
// Manages most of the other core components
class Game 
{
	static Game* GameInstance;

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize(int width, int height);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);

	Renderer* GetRenderer();

private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadTextures();
	void LoadShaders();
	void CreateBasicGeometry();

	Renderer m_renderer;

	// game input
	GameInput* inputManager;

	void acquireInput();


	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	float time = 0;
	bool reversed = false;

	float timeShot = -1;

	ClientManager* clientInterface;

	// static callbacks for the Function Pointers
	static void SUpdate(float deltaTime, float totalTime);
	static void SDraw(float deltaTime, float totalTime);
	static void SOnResize(int width, int height);
	static void SOnMouseDown(WPARAM buttonState, int x, int y);
	static void SOnMouseUp(WPARAM buttonState, int x, int y);
	static void SOnMouseMove(WPARAM buttonState, int x, int y);
	static void SOnMouseWheel(float wheelDelta, int x, int y);
};

