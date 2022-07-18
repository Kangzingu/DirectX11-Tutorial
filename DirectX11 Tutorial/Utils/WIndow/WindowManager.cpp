#include "WindowManager.h"
#include <memory>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void WindowManager::Initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int windowWidth, int windowHeight)
{
	/* 윈도우 초기화 */
	this->window.Initialize(this, hInstance, windowTitle, windowClass, windowWidth, windowHeight);

	/* 입력 장치?(Raw Input Device..) 초기화 */
	static bool isRawInputDeviceRegistered = false;
	if (!isRawInputDeviceRegistered)
	{
		RAWINPUTDEVICE rawInputDevice;
		rawInputDevice.usUsagePage = 0x01;// 0x01 요게 마우스의 디바이스 코드? 인듯
		rawInputDevice.usUsage = 0x02;
		rawInputDevice.dwFlags = 0;
		rawInputDevice.hwndTarget = NULL;
		if (!RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)))
		{
			ErrorLogger::Log(GetLastError(), "Failed to register raw input deivces");
			exit(-1);
		}
		isRawInputDeviceRegistered = true;
	}	
}

LRESULT WindowManager::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// ImGui Proc Handler에 우리의 모든 메시지를 전달해줌, ImGui 관련 메시지를 처리할 수 있겡
	// ImGui 관련 메시지라면 얘가 처리하고 return으로 걍 끝내버리고, 관련 메시지가 아니라면 기존에 메시지 처리하던 switch문으로 넘어가겠징
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
	{
		return true;
	}
	switch (uMsg)
	{
		// 키보드 이벤트 처리
		case WM_KEYDOWN:
		{
			unsigned char keycode = static_cast<unsigned char>(wParam);
			if (keyboard.IsKeysAutoRepeat())
			{
				keyboard.OnKeyPressed(keycode);
			}
			else
			{
				const bool wasPressed = lParam & 0x40000000;
				if (!wasPressed)
				{
					keyboard.OnKeyPressed(keycode);
				}
			}
			return 0;
		}
		case WM_KEYUP:
		{
			unsigned char keycode = static_cast<unsigned char>(wParam);
			keyboard.OnKeyReleased(keycode);
			return 0;
		}
		case WM_CHAR:
		{
			unsigned char ch = static_cast<unsigned char>(wParam);
			if (keyboard.IsCharsAutoRepeat())
			{
				keyboard.OnChar(ch);
			}
			else
			{
				const bool wasPressed = lParam & 0x40000000;
				if (!wasPressed)
				{
					keyboard.OnChar(ch);
				}
			}
			return 0;
		}

		// 마우스 이벤트 처리
		case WM_MOUSEMOVE:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnMouseMove(x, y);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnLeftPressed(x, y);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnRightPressed(x, y);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnMiddlePressed(x, y);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnLeftReleased(x, y);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnRightReleased(x, y);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnMiddleReleased(x, y);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				mouse.OnWheelUp(x, y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				mouse.OnWheelDown(x, y);
			}
			return 0;
		}
		case WM_INPUT:
		{
			UINT dataSize;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
			if (dataSize > 0)
			{
				std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
				{
					RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
					if (raw->header.dwType == RIM_TYPEMOUSE)
					{
						mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
					}
				}
			}
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}