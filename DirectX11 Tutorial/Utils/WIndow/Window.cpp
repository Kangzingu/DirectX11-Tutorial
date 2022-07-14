#include "WindowManager.h"

bool Window::Initialize(WindowManager* pWindowContainer, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int windowWidth, int windowHeight)
{
	this->hInstance = hInstance;// 윈도우 운영체제에서 실행되는 프로그램들을 구별하기 위한 ID 값
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	this->windowTitle = windowTitle;
	this->windowTitleWString = StringHelper::StringToWide(windowTitle);
	this->windowClass = windowClass;
	this->windowClassWString =StringHelper::StringToWide(windowClass);

	this->RegisterWindowClass();

	RECT windowRect;
	windowRect.left = 100;
	windowRect.top = 100;
	windowRect.right = windowRect.left + this->windowWidth;
	windowRect.bottom = windowRect.top + this->windowHeight;
	AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	this->handle = CreateWindowEx(0,
								  windowClassWString.c_str(),
								  windowTitleWString.c_str(),
								  WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
								  windowRect.left,
								  windowRect.top,
								  windowRect.right - windowRect.left,
								  windowRect.bottom - windowRect.top,
								  NULL,
								  NULL,
								  this->hInstance,
								  pWindowContainer);

	if (this->handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), "윈도우 생성에 실패했습니다. 윈도우명: " + this->windowTitle);
		return false;
	}

	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}

bool Window::ProcessMessages()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// PeekMessage는 메시지가 있다면 true, 없으면 false를 리턴
	while (PeekMessage(&msg, this->handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow(this->handle))
		{
			this->handle = NULL;
			UnregisterClass(windowClassWString.c_str(), this->hInstance);
			return false;
		}
	}

	return true;
}

HWND Window::GetHWND() const
{
	return this->handle;
}

Window::~Window()
{
	if (this->handle != NULL)
	{
		UnregisterClass(windowClassWString.c_str(), this->hInstance);
		DestroyWindow(handle);
	}
}

int Window::GetWidth()
{
	return windowWidth;
}

int Window::GetHeight()
{
	return windowHeight;
}

LRESULT HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;
		default:
		{
			WindowManager* const pWindow = reinterpret_cast<WindowManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_NCCREATE:
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			WindowManager* pWindow = reinterpret_cast<WindowManager*>(pCreate->lpCreateParams);
			if (pWindow == nullptr)
			{
				ErrorLogger::Log("윈도우 컨테이너에 대한 포인터가 NULL임(WM_NCCREATE 생성 간 발생)");
				exit(-1);
			}
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
			return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void Window::RegisterWindowClass()
{
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->windowClassWString.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);
}