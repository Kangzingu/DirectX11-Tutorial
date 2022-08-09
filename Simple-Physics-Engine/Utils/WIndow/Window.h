#pragma once
#include "../ErrorLogger.h"

class WindowManager;
class Window
{
private:
	HINSTANCE m_hInstance = NULL;
	std::string m_title = "";
	std::wstring m_titleWString = L"";
	std::string m_class = "";
	std::wstring m_classWString = L"";
	int m_width = 0;
	int m_height = 0;

	HWND m_handle = NULL;

	bool m_isEnable;

public:
	void Initialize(WindowManager* pWindowContainer, HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int windowWidth, int windowHeight);
	void HandleMessage();

	bool IsEnable();

	HWND GetHWND() const;
	int GetWidth();
	int GetHeight();

	~Window();
};