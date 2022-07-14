#pragma once
#include "WindowContainer.h"
#include "Timer.h"

class Engine :WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int windowWidth, int windowHeight);
	bool ProcessMessages();
	void Update();
	void RenderFrame();
private:
	Timer sceneTimer;
	float deltaTime;
};