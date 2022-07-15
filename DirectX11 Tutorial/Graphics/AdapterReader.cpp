#include "AdapterReader.h"

// static멤버인데 클래스에서 한번도 안쓰면? 이렇게 선언해 줘야 하는듯.. 별 의미있진 않아보임
std::vector<AdapterData> AdapterReader::adapters;

// 그냥 DXGIFactory 얘 사용해서 데이터 가져오는것인듯
std::vector<AdapterData> AdapterReader::GetAdapters() {
	if (adapters.size() > 0)
		return adapters;
	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
	ERROR_IF_FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf())),
					"DXGIFactory 생성 실패");
	IDXGIAdapter* pAdapter;
	UINT index = 0;
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter))) {
		adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	ERROR_IF(!adapters.size() > 0,
			 "IDXGI Adapter를 찾지 못했습니다");
	return adapters;
}

// 데이터 초기화.. 입력받은 Adapter를 저장하고 그에대한 정보를 description에 저장
AdapterData::AdapterData(IDXGIAdapter* pAdapter) {
	this->pAdapter = pAdapter;
	ERROR_IF_FAILED(pAdapter->GetDesc(&this->description),
					"AdapterData를 정상적으로 초기화하지 못했습니다(Adapter에 대한 description을 받아오지 못함)");
}