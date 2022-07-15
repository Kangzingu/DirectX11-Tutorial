#include "AdapterReader.h"

// static����ε� Ŭ�������� �ѹ��� �Ⱦ���? �̷��� ������ ��� �ϴµ�.. �� �ǹ����� �ʾƺ���
std::vector<AdapterData> AdapterReader::adapters;

// �׳� DXGIFactory �� ����ؼ� ������ �������°��ε�
std::vector<AdapterData> AdapterReader::GetAdapters() {
	if (adapters.size() > 0)
		return adapters;
	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
	ERROR_IF_FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf())),
					"DXGIFactory ���� ����");
	IDXGIAdapter* pAdapter;
	UINT index = 0;
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter))) {
		adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	ERROR_IF(!adapters.size() > 0,
			 "IDXGI Adapter�� ã�� ���߽��ϴ�");
	return adapters;
}

// ������ �ʱ�ȭ.. �Է¹��� Adapter�� �����ϰ� �׿����� ������ description�� ����
AdapterData::AdapterData(IDXGIAdapter* pAdapter) {
	this->pAdapter = pAdapter;
	ERROR_IF_FAILED(pAdapter->GetDesc(&this->description),
					"AdapterData�� ���������� �ʱ�ȭ���� ���߽��ϴ�(Adapter�� ���� description�� �޾ƿ��� ����)");
}