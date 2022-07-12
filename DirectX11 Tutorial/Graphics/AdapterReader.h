#pragma once
// 왜 "ErrorLogger.h"를 추가한진 모르겠움, string 이랑 windows.h 일케 드가있는데..
#include "../ErrorLogger.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

// 물리적인(GPU 상태 같은거..?) 데이터를 연결하고 관리해주는 클래스인 듯?
class AdapterData {
public:
	// DXGI(Directx Graphics Infrastructure): 이게 붙은애들은 저수준 작업(ex, 스왑체인, 해상도, 주사율 등)을 관리하고 관련 데이터를 갖고있는 넘들인듯
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