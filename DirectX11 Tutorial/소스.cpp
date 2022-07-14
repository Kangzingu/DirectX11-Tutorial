#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	HRESULT hr = CoInitialize(NULL);// �ؽ��� �ε��ϴ� ���̺귯�� "WICTextureLoader.h"�꿡�� �ؽ��� �ε������ �����۵� �Ϸ��� �̰� �ʿ��ѵ�
	COM_ERROR_IF_FAILED(hr, "CoInitialize ȣ�⿡ �����߽��ϴ�");

	Engine engine;
	int windowWidth = 800;
	int windowHeight = 600;

	if (engine.Initialize(hInstance, "Simple Physics Engine", "MyWindowClass", 800, 600))
	{
		while (engine.ProcessMessages() == true)
		{
			engine.Update();
			engine.RenderFrame();
		}
	}
	return 0;
}

/* �츮�� ���� ��(�Ƹ���?) ���� */
// 1. Input-Assembler - ��
// 2. Vertex Shader - ��
// 3. Rasterizer - ��
// 4. Pixel Shader - ��
// 5. Output-Merger - ��

// Create out vertex buffer
// Draw