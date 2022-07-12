#pragma once
// �� "ErrorLogger.h"�� �߰����� �𸣰ڿ�, string �̶� windows.h ���� �尡�ִµ�..
#include "../ErrorLogger.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

// ��������(GPU ���� ������..?) �����͸� �����ϰ� �������ִ� Ŭ������ ��?
class AdapterData {
public:
	// DXGI(Directx Graphics Infrastructure): �̰� �����ֵ��� ������ �۾�(ex, ����ü��, �ػ�, �ֻ��� ��)�� �����ϰ� ���� �����͸� �����ִ� �ѵ��ε�
	AdapterData(IDXGIAdapter* pAdapter);
	IDXGIAdapter* pAdapter = nullptr;
	DXGI_ADAPTER_DESC description;
};

class AdapterReader {
public:
	static std::vector<AdapterData> GetAdapters();
private:
	static std::vector<AdapterData> adapters;
};