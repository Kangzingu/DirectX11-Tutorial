#pragma once
#include "ErrorLogger.h"

class WindowManager;

class Window {
public:
	bool Initialize(WindowManager* pWindowContainer, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int windowWidth, int windowHeight);
	bool ProcessMessages();
	HWND GetHWND() const;
	~Window();
	int GetWidth();
	int GetHeight();
private:
	void RegisterWindowClass();
	HWND handle = NULL;
	HINSTANCE hInstance = NULL;
	int windowWidth = 0;
	int windowHeight = 0;
	std::string windowTitle = "";
	std::string windowClass = "";
	std::wstring windowTitleWString = L"";
	std::wstring windowClassWString = L"";
};