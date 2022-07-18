#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	ERROR_IF_FAILED(CoInitialize(NULL),
					"CoInitialize 호출에 실패했습니다");

	Engine engine;
	engine.Initialize(hInstance);
	engine.Run();

	return 0;
}