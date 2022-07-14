#pragma once
#include "WindowManager.h"
#include "Graphics/Renderer.h"

class Engine
{
public:
	bool InitializeWindow(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int windowWidth, int windowHeight);
	bool InitializeRenderer();
	bool IsAlive();
	void Run();
	void RenderFrame();
private:
	Timer sceneTimer;
	float deltaTime;
	Renderer renderer;// GFX == Renderer Effect
	WindowManager windowManager;
};