#include "AdapterReader.h"

// static����ε� Ŭ�������� �ѹ��� �Ⱦ���? �̷��� ������ ��� �ϴµ�.. �� �ǹ����� �ʾƺ���
std::vector<AdapterData> AdapterReader::adapters;

// �׳� DXGIFactory �� ����ؼ� ������ �������°��ε�
std::vector<AdapterData> AdapterReader::GetAdapters() {
	// adapters�� �ʱ�ȭ �Ǿ��ִٸ� �׳� �װ� ����
	if (adapters.size() > 0)
		return adapters;

	// adapters�� �ʱ�ȭ �ȵǾ��ִٸ� �ʱ�ȭ �� ��
	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
	
	// DXGIFactory ��ü�� ����. �׳� DXGI �������̽��� ������ �̰� ������ �Ѵٰ� ��
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "DXGIFactory ���� ����");
		exit(-1);
	}

	IDXGIAdapter* pAdapter;
	UINT index = 0;
	
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter))) {
		adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	return adapters;
}

// ������ �ʱ�ȭ.. �Է¹��� Adapter�� �����ϰ� �׿����� ������ description�� ����
AdapterData::AdapterData(IDXGIAdapter* pAdapter) {
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&this->description);
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "AdapterData�� ���������� �ʱ�ȭ���� ���߽��ϴ�(Adapter�� ���� description�� �޾ƿ��� ����)");
	}
}