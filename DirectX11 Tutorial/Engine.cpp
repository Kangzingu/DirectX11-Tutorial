#include "Engine.h"
// 엔진이라는건 없어도 댈 거 같아요.
void Engine::Run()
{
	while (IsAlive())
	{
		UpdateDeltaTime();
		ManageInput();

		// renderer.UpdatePhysics()
		// renderer.UpdatePhysics()
		renderer.Render();

	}
}

void Engine::InitializeWindow(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int windowWidth, int windowHeight)
{
	if (!windowManager.Initialize(hInstance, windowTitle, windowClass, windowWidth, windowHeight))
		ErrorLogger::Log("WindowManager 초기화 실패");
}

void Engine::InitializeRenderer(void)
{
	sceneTimer.Start();

	if (!renderer.Initialize(windowManager.window.GetHWND(), windowManager.window.GetWidth(), windowManager.window.GetHeight()))
		ErrorLogger::Log("Renderer 초기화 실패");
}


void Engine::ManageInput()
{
	while (!windowManager.keyboard.IsCharBufferEmpty())
	{
		unsigned char ch = windowManager.keyboard.ReadChar();
	}
	while (!windowManager.keyboard.IsKeyBufferEmpty())
	{
		KeyboardEvent kbe = windowManager.keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	while (!windowManager.mouse.IsEventBufferEmpty())
	{
		MouseEvent me = windowManager.mouse.ReadEvent();
		if (windowManager.mouse.IsRightDown() == true)
		{
			if (me.GetType() == MouseEvent::Type::RAW_MOVE)
			{
				this->renderer.camera3D.AdjustRotation((float)me.GetPosY() * 0.001f, (float)me.GetPosX() * 0.001f, 0);
			}
		}
	}

	this->renderer.gameObject.AdjustRotation(0.0f, 0.001f * deltaTime, 0.0f);

	float Camera3DSpeed = 0.008f;

	if (windowManager.keyboard.KeyIsPressed(VK_SHIFT))
	{
		Camera3DSpeed = 0.3f;
	}
	if (windowManager.keyboard.KeyIsPressed('W'))
	{
		this->renderer.camera3D.AdjustPosition(this->renderer.camera3D.GetForwardVector() * Camera3DSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed('A'))
	{
		this->renderer.camera3D.AdjustPosition(this->renderer.camera3D.GetLeftVector() * Camera3DSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed('S'))
	{
		this->renderer.camera3D.AdjustPosition(this->renderer.camera3D.GetBackwardVector() * Camera3DSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed('D'))
	{
		this->renderer.camera3D.AdjustPosition(this->renderer.camera3D.GetRightVector() * Camera3DSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed(VK_SPACE))
	{
		this->renderer.camera3D.AdjustPosition(0.0f, Camera3DSpeed * deltaTime, 0.0f);
	}
	if (windowManager.keyboard.KeyIsPressed(VK_CONTROL))
	{
		this->renderer.camera3D.AdjustPosition(0.0f, -Camera3DSpeed * deltaTime, 0.0f);
	}
	if (windowManager.keyboard.KeyIsPressed('C'))
	{
		XMVECTOR lightPosition = this->renderer.camera3D.GetPositionVector();
		lightPosition += this->renderer.camera3D.GetForwardVector();
		this->renderer.light.SetPosition(lightPosition);
		this->renderer.light.SetRotation(this->renderer.camera3D.GetRotationFloat3());
	}
}
bool Engine::IsAlive()
{
	return this->windowManager.window.ProcessMessages();
}

void Engine::UpdateDeltaTime()
{
	deltaTime = sceneTimer.GetElapsedMiliseconds();
	sceneTimer.Restart();
}