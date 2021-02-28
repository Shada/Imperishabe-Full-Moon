#pragma once
#include "stdafx.h"
#include <dinput.h>
class InputHandler
{
private:
	
	HRESULT					initDInput( HINSTANCE hInstance, HWND* hWnd );

	LPDIRECTINPUT8			din;

	LPDIRECTINPUTDEVICE8	dinKeyboard;
	LPDIRECTINPUTDEVICE8	dinMouse;

	char					keyState[256], prevKeyState[256];

	DIMOUSESTATE			mouseState, prevMouseState;

public:
	InputHandler( HINSTANCE hInstance, HWND* hWnd );
	~InputHandler();

	void Update();
	
	DIMOUSESTATE *getMouseState() { return &mouseState;}
	DIMOUSESTATE *getPrevMouseState() { return &prevMouseState;}
	char* getKeyState()	{ return keyState; }
	char* getPrevKeyState()	{ return prevKeyState; }

	long getDeltaY()	{ return mouseState.lY; }
	long getDeltaX()	{ return mouseState.lX; }
	long getDeltaZ()	{ return mouseState.lZ; }
};

