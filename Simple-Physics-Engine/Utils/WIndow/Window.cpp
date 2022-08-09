#include "WindowManager.h"

LRESULT HandleMessageRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		DestroyWindow(hwnd);
		return 0;
	}
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
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMessageRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

void Window::Initialize(WindowManager* pWindowContainer, HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int windowWidth, int windowHeight)
{
	m_hInstance = hInstance;// 윈도우 운영체제에서 실행되는 프로그램들을 구별하기 위한 ID 값
	m_title = windowTitle;
	m_titleWString = StringHelper::StringToWString(windowTitle);
	m_class = windowClass;
	m_classWString = StringHelper::StringToWString(windowClass);
	m_width = windowWidth;
	m_height = windowHeight;
	m_isEnable = true;

	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_classWString.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);

	RECT windowRect;
	windowRect.left = 30;
	windowRect.top = 30;
	windowRect.right = windowRect.left + m_width;
	windowRect.bottom = windowRect.top + m_height;
	AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	m_handle = CreateWindowEx(0,
							  m_classWString.c_str(),
							  m_titleWString.c_str(),
							  WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
							  windowRect.left,
							  windowRect.top,
							  windowRect.right - windowRect.left,
							  windowRect.bottom - windowRect.top,
							  NULL,
							  NULL,
							  m_hInstance,
							  pWindowContainer);
	if (m_handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), "윈도우 생성에 실패했습니다. 윈도우명: " + m_title);
		return;
	}

	ShowWindow(m_handle, SW_SHOW);
	SetForegroundWindow(m_handle);
	SetFocus(m_handle);
}
void Window::HandleMessage()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (PeekMessage(&msg, m_handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_NULL)
	{
		if (IsWindow(m_handle) == false)
		{
			m_handle = NULL;
			UnregisterClass(m_classWString.c_str(), m_hInstance);
			m_isEnable = false;
		}
	}
}

HWND Window::GetHWND() const { return m_handle; }
int Window::GetWidth() { return m_width; }
int Window::GetHeight() { return m_height; }
bool Window::IsEnable() { return m_isEnable; }
Window::~Window()
{
	if (m_handle != NULL)
	{
		UnregisterClass(m_classWString.c_str(), m_hInstance);
		DestroyWindow(m_handle);
	}
}