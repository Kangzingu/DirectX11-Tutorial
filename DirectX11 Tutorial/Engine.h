#pragma once
#include "Utils/WIndow/WindowManager.h"
#include "Graphics/Renderer.h"

class Engine
{
public:
	void InitializeWindow(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int windowWidth, int windowHeight);
	void InitializeRenderer(void);
	void Run();
	bool IsAlive();
	void UpdateDeltaTime();
	void ManageInput();
private:
	Timer sceneTimer;
	float deltaTime;
	Renderer renderer;// GFX == Renderer Effect
	WindowManager windowManager;
};
