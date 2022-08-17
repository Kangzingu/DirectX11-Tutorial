#include "Scene.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	ERROR_IF_FAILED(CoInitialize(NULL),
					"CoInitialize 호출에 실패했습니다");

	Scene scene;
	scene.Initialize(hInstance);
	scene.Run();

	return 0;
}

/*
 1. 조건식에 !연산자는 사용 x
 2. SAL / noexcept 사용 금지(OUT, INOUT은 필요에 따라 사용 가능)
 3. 클래스 선언
	class C
	{
	public:
		//변수

	public:
		//함수

	protected:
		//변수

	protected:
		//함수

	private:
		//변수

	private:
		//함수
	};

 4. 주석은 최대한 없도록
 5. 클래스 초기화 등 함수 이름
 - createActor() / destroyActor()
 - initializeActor()
 6. 약자는 모두 대문자로 ex) ui -> UI
 7. 들여쓰기 최대한 안하게

- 나쁜예
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

 - 좋은예
bool process()
{
	if (hasBall == false)
		return false;

	for (int i = 0; i < length; i++)
	{
		// do
	}
}

 8. 벡터 순회 방법

- 나쁜예
for (int i = 0; i < numItems.size(); i++)
{
	// do
}

 - 좋은예
 for (Item& item : items)
{
	// do
}
*/