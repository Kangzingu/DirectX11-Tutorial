#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	COM_ERROR_IF_FAILED(CoInitialize(NULL), "CoInitialize 호출에 실패했습니다");

	static std::string windowTitle = "Simple Physics Engine";
	std::string windowClass = "Default";
	int windowWidth = 800;
	int windowHeight = 600;

	Engine engine;
	engine.InitializeWindow(hInstance, windowTitle, windowClass, windowWidth, windowHeight);
	engine.InitializeRenderer();
	engine.Run();

	return 0;
}