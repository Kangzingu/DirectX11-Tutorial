#pragma once
#include "Window.h"
#include "Keyboard/Keyboard.h"
#include "Mouse/Mouse.h"
#include "Graphics/Renderer.h"

class WindowContainer {
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	Window window;
	Keyboard keyboard;
	Mouse mouse;
	Renderer renderer;// GFX == Renderer Effect
};