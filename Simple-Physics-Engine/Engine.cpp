#include "Scene.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	ERROR_IF_FAILED(CoInitialize(NULL),
					"CoInitialize ȣ�⿡ �����߽��ϴ�");

	Scene scene;
	scene.Initialize(hInstance);
	scene.Run();

	return 0;
}

/*
 1. ���ǽĿ� !�����ڴ� ��� x
 2. SAL / noexcept ��� ����(OUT, INOUT�� �ʿ信 ���� ��� ����)
 3. Ŭ���� ����
	class C
	{
	public:
		//����

	public:
		//�Լ�

	protected:
		//����

	protected:
		//�Լ�

	private:
		//����

	private:
		//�Լ�
	};

 4. �ּ��� �ִ��� ������
 5. Ŭ���� �ʱ�ȭ �� �Լ� �̸�
 - createActor() / destroyActor()
 - initializeActor()
 6. ���ڴ� ��� �빮�ڷ� ex) ui -> UI
 7. �鿩���� �ִ��� ���ϰ�

- ���ۿ�
bool process()
{
	if (hasBall == true)
	{
		for (int i = 0; i < length; i++)
		{
			// do
		}
	}
}

 - ������
bool process()
{
	if (hasBall == false)
		return false;

	for (int i = 0; i < length; i++)
	{
		// do
	}
}

 8. ���� ��ȸ ���

- ���ۿ�
for (int i = 0; i < numItems.size(); i++)
{
	// do
}

 - ������
 for (Item& item : items)
{
	// do
}
*/