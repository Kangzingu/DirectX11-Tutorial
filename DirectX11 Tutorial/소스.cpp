#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	// 텍스쳐 로딩하는 라이브러리 "WICTextureLoader.h"얘에서 텍스쳐 로딩기능이 정상작동 하려면 이게 필요한듯
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "FAILED to call CoInitialize");
		return -1;
	}

	Engine engine;
	if (engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600))
	{
		while (engine.ProcessMessages() == true)
		{
			engine.Update();
			engine.RenderFrame();
		}
	}
	return 0;
}

/* 우리가 만들 것(아마도?) 참고 */
// 1. Input-Assembler - 끝
// 2. Vertex Shader - 끝
// 3. Rasterizer - 끝
// 4. Pixel Shader - 끝
// 5. Output-Merger - 끝

// Create out vertex buffer
// Draw