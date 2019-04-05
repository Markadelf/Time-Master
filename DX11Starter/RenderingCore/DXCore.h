#pragma once

// NOT UNNECESSARY
#include "Winsock2.h"
// NOT UNNECESSARY

#include <Windows.h>
#include <d3d11.h>
#include <string>

// We can include the correct library files here
// instead of in Visual Studio settings if we want
#pragma comment(lib, "d3d11.lib")


class DXCore
{
public:
	DXCore(
		HINSTANCE hInstance,		// The application's handle
		char* titleBarText,			// Text for the window's title bar
		unsigned int windowWidth,	// Width of the window's client area
		unsigned int windowHeight,	// Height of the window's client area
		bool debugTitleBarStats);	// Show extra stats (fps) in title bar?
	~DXCore();

	// Static requirements for OS-level message processing
	static DXCore* DXCoreInstance;
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,		// Window handle
		UINT uMsg,		// Message
		WPARAM wParam,	// Message's first parameter
		LPARAM lParam	// Message's second parameter
		);

	// Internal method for message handling
	LRESULT ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Initialization and game-loop related methods
	HRESULT InitWindow();
	HRESULT InitDirectX();
	HRESULT Run();				
	void Quit();
	virtual void OnResize();
	
	// Pure virtual methods for setup and game functionality
	virtual void Init()										= 0;
	
	// Functions to set function pointers
	void SetUpdate(void(*callback)(float deltaTime, float totalTime));
	void SetDraw(void (*callback)(float deltaTime, float totalTime));
	void SetControls(
		void(*msDown)(WPARAM buttonState, int x, int y), 
		void(*msUp)(WPARAM buttonState, int x, int y), 
		void(*msMove)(WPARAM buttonState, int x, int y),
		void(*msWheel)(float wheelDelta, int x, int y));
	
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	HWND* GethWnd();

protected:
	HINSTANCE	hInstance;		// The handle to the application
	HWND		hWnd;			// The handle to the window itself
	std::string titleBarText;	// Custom text in window's title bar
	bool		titleBarStats;	// Show extra stats in title bar?
	
	// Size of the window's client area
	unsigned int width;
	unsigned int height;
	
	// DirectX related objects and variables
	D3D_FEATURE_LEVEL		dxFeatureLevel;
	IDXGISwapChain*			swapChain;
	ID3D11Device*			device;
	ID3D11DeviceContext*	context;

	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* depthStencilView;

	// Update function pointers
	void (*update)(float deltaTime, float totalTime);
	void (*draw)(float deltaTime, float totalTime);
	
	// Convenience methods for handling mouse input, since we
	// can easily grab mouse input from OS-level messages
	void (*onMouseDown)(WPARAM buttonState, int x, int y);
	void (*onMouseUp)(WPARAM buttonState, int x, int y);
	void (*onMouseMove)(WPARAM buttonState, int x, int y);
	void (*onMouseWheel)(float wheelDelta, int x, int y);

	// Helper function for allocating a console window
	void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);

private:
	// Timing related data
	double perfCounterSeconds;
	float totalTime;
	float deltaTime;
	__int64 startTime;
	__int64 currentTime;
	__int64 previousTime;

	// FPS calculation
	int fpsFrameCount;
	float fpsTimeElapsed;
	
	void UpdateTimer();			// Updates the timer for this frame
	void UpdateTitleBarStats();	// Puts debug info in the title bar
};

