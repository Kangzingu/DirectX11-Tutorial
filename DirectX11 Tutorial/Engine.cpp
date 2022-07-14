#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int windowWidth, int windowHeight) {
	/* 타이머 켜고                 */
	sceneTimer.Start();
	/* 창 띄우고                   */
	if (!window.Initialize(this, hInstance, windowTitle, windowClass, windowWidth, windowHeight))
	{
		return false;
	}
	/* 그래픽 관련된것 초기화 하고 */
	if (!renderer.Initialize(window.GetHWND(), windowWidth, windowHeight))
	{
		return false;
	}

	return true;
}

bool Engine::ProcessMessages() {
	return this->window.ProcessMessages();
}

void Engine::Update() {
	/* Delta Time 업데이트*/
	deltaTime = sceneTimer.GetElapsedMiliseconds();
	sceneTimer.Restart();

	
	while (!keyboard.IsCharBufferEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}
	while (!keyboard.IsKeyBufferEmpty()) {
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	while (!mouse.IsEventBufferEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown() == true)
		{
			if (me.GetType() == MouseEvent::Type::RAW_MOVE)
			{
				this->renderer.camera3D.AdjustRotation((float)me.GetPosY() * 0.001f, (float)me.GetPosX() * 0.001f, 0);
			}
		}
	}

	this->renderer.gameObject.AdjustRotation(0.0f, 0.001f * deltaTime, 0.0f);

	float Camera3DSpeed = 0.008f;

	if (keyboard.KeyIsPressed(VK_SHIFT))
	{
		Camera3DSpeed = 0.3f;
	}

	if (keyboard.KeyIsPressed('W'))
	{
		this->renderer.camera3D.AdjustPosition(this->renderer.camera3D.GetForwardVector() * Camera3DSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->renderer.camera3D.AdjustPosition(this->renderer.camera3D.GetBackwardVector() * Camera3DSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->renderer.camera3D.AdjustPosition(this->renderer.camera3D.GetLeftVector() * Camera3DSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->renderer.camera3D.AdjustPosition(this->renderer.camera3D.GetRightVector() * Camera3DSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed(VK_SPACE))
	{
		this->renderer.camera3D.AdjustPosition(0.0f, Camera3DSpeed * deltaTime, 0.0f);
	}
	if (keyboard.KeyIsPressed('Z'))
	{
		this->renderer.camera3D.AdjustPosition(0.0f, -Camera3DSpeed * deltaTime, 0.0f);
	}
	if (keyboard.KeyIsPressed('C'))
	{
		XMVECTOR lightPosition = this->renderer.camera3D.GetPositionVector();
		lightPosition += this->renderer.camera3D.GetForwardVector();
		this->renderer.light.SetPosition(lightPosition);
		this->renderer.light.SetRotation(this->renderer.camera3D.GetRotationFloat3());
	}
}

void Engine::RenderFrame()
{
	renderer.RenderFrame();
}
