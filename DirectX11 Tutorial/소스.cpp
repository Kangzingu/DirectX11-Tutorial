#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	COM_ERROR_IF_FAILED(CoInitialize(NULL), "CoInitialize ȣ�⿡ �����߽��ϴ�");

	Engine engine;
	std::string windowTitle = "Simple Physics Engine";
	std::string windowClass = "Default";
	int windowWidth = 800;
	int windowHeight = 600;

	if (!engine.InitializeWindow(hInstance, windowTitle, windowClass, windowWidth, windowHeight))
		return -1;
	if (!engine.InitializeRenderer())
		return -1;
	while (engine.IsAlive())
	{
		engine.Run();
	}

	return 0;
}