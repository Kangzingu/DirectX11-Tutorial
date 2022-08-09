#pragma once
#include "Window.h"
#include "Keyboard.h"
#include "Mouse.h"

class WindowManager
{
public:
	Window m_window;
	Keyboard m_keyboard;
	Mouse m_mouse;

public:
	WindowManager(HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int windowWidth, int windowHeight);
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};