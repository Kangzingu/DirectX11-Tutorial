#include "Graphics.h"

/* DirectX 11 그래픽스 파이프라인 참고(https://www.youtube.com/watch?v=4RFlOvVPuys)*/
// 1. Input-Assembler => Not Programmable
// 2. Vertex Shader
// 3. Tesselation
// 4. Geometry Shader
// 5. Rasterizer => Not Programmable
// 6. Pixel Shader
// 7. Output-Merge => Not Programmable
// 8. Render Target

/* 우리가 만들 것(아마도?) 참고 */
// 1. Input-Assembler
// 2. Vertex Shader
// 3. Resterizer
// 4. Pixel Shader
// 5. Output-Merger

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;

	this->fpsTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	// Setup ImGui

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 이거랑

	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);

	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	UINT offset = 0;
	
	// Constant Buffer 업데이트
	XMMATRIX world = XMMatrixIdentity();
	constantBuffer.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
	if (!constantBuffer.ApplyChanges())
		return;
	this->deviceContext->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());

	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	// 인덱스 버퍼 데이터형이 DWORD(32비트)니까 R32(R은 RGB이거니까 별 의미 없는거 같고 32비트짜리 1개를 뜻하는게 이거라 이걸로 한거가틈)로 한듯
	this->deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0); // 이거 바꾸면 다르게 그려짐

	// FPS 세서 띄우기
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringConverter::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	// ImGui 프레임 시작
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// ImGui 테스트 윈도우 생성
	ImGui::Begin("Test");
	ImGui::End();
	// 그릴 데이터 모으기
	ImGui::Render();
	// 그리기
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapchain->Present(0, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	// 그래픽 카드 정보 읽어옴

	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log("IDXGI Adapter를 찾지 못했습니다");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// backbuffer의 display mode에 대한 옵션
	scd.BufferDesc.Width = windowWidth;
	scd.BufferDesc.Height = windowHeight;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// 색+데이터에 할당할 바이트 수? 관련된 것인듯
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// multi-sampling 파라미터
	scd.SampleDesc.Count = 1;// 1은 멀티 샘플링을 하지 않겠다는 것
	scd.SampleDesc.Quality = 0;// 얘도 마찬가지

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	scd.BufferCount = 1;// 이중 버퍼링: 1, 삼중 버퍼링: 2

	scd.OutputWindow = hwnd;

	scd.Windowed = TRUE;

	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter,// IDXGI Adapter
									   D3D_DRIVER_TYPE_UNKNOWN,
									   NULL,// 소프트웨어 드라이버 타입
									   NULL,// runtime layers를 위한 flags
									   NULL,// feature levels array
									   0,// array 내 feature level의 #(갯수인듯?)
									   D3D11_SDK_VERSION,
									   &scd,// 스왑체인 description
									   this->swapchain.GetAddressOf(),// 스왑체인 주소
									   this->device.GetAddressOf(),// 디바이스 주소
									   NULL,// 지원되는 feature level
									   this->deviceContext.GetAddressOf());// 디바이스 컨텍스트 주소
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "디바이스와 스왑체인 생성에 실패했습니다");
		return false;
	}

	// 렌더 타겟(DirectX에서 렌더 타겟은 백버퍼를 의미하는 듯) 관련..?
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "스왑체인의 GetBuffer 함수 호출에 실패했습니다");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "렌더타겟 뷰 생성에 실패했습니다");
		return false;
	}

	// Depth/Stencil Buffer(z값 체크해서 젤 가까운것만 그려주는겅) 관련("OMSetRenderTargets" 전에 와야하는 듯? 근데 이거 뒤에도 있음..)
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = windowWidth;
	depthStencilDesc.Height = windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Depth Stencil 텍스쳐 생성에 실패했습니다");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Depth Stencil 뷰 생성에 실패했습니다");
		return false;
	}

	// 이제 렌더 타겟을 설정? 채우기? 만 하면 댐. 이게 Output Merger 단계를 위한 준비의 다인듯?
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	// Depth/Stencil State 생성. 위에 depthStencilDesc랑 이름이 너무 똑같다.. 이건 꼭 바꿔주자..
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;//D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;// 일케하면 스텐실 꺼짐
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;// 일케하면 깊이가 같은 경우에는 걍 넘어가는듯, LESS_EQUAL은 같은 경우 늦게 들어온넘을 그리고

	hr = this->device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Depth Stencil 상태 생성에 실패했습니다");
		return false;
	}

	// 뷰포트 및 라스터라이저 상태 생성. 이게 Rasterizer 단계를 위한 준비의 다인듯
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = windowWidth;
	viewport.Height = windowHeight;
	viewport.MinDepth = 0.0f; // 카메라에 가깝
	viewport.MaxDepth = 1.0f; // 카메라에 멈

	this->deviceContext->RSSetViewports(1, &viewport);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;//D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;//D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // 참고로 OpenGL은 ccw, DirectX는 cw임
	//rasterizerDesc.FrontCounterClockwise = true;

	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "라스터라이저 상태 생성에 실패했습니다");
		return false;
	}

	// 글자 관련 코드
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data/Fonts/comic_sans_ms_16.spritefont");

	// 텍스쳐 관련 코드

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	// 일케 LoD 설정하면 최소부터 최대까지 전 범위 다 커버되는듯
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "샘플러 상태 생성에 실패했습니다");
		return false;
	}

	return true;
}

bool Graphics::InitializeShaders()
{

	// 쉐이더를 읽어와야하니 그 경로정보를 전처리해줌
	std::wstring shaderfolder = L"";
#pragma region DetemineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG // 디버그 모드
#ifdef _WIN64 // x64
		shaderfolder = L"../x64/Debug/";
#else // x86
		shaderfolder = L"../Debug/";
#endif
#else // 릴리즈 모드
#ifdef _WIN64 // x64
		shaderfolder = L"../x64/Release/";
#else // x86
		shaderfolder = L"../Release/";
#endif
#endif
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexshader.Initialize(this->device, shaderfolder + L"vertexshader.cso", layout, numElements))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"pixelshader.cso"))
		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	Vertex v[] = {
		Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f),
		Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f),
		Vertex(0.5f, 0.5f, 0.0f, 1.0f, 0.0f),
		Vertex(0.5f, -0.5f, 0.0f, 1.0f, 1.0f),
	};

	HRESULT hr = this->vertexBuffer.Initialize(this->device.Get(), v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "버텍스 버퍼 생성에 실패했습니다");
		return false;
	}
	DWORD indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	hr = this->indicesBuffer.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "인덱스 버퍼 생성에 실패했습니다");
		return hr;
	}

	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/TestTexture.png", nullptr, myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "이미지 파일로부터 WIC 텍스쳐 생성에 실패했습니다");
		return false;
	}

	// Constant Buffer 초기화
	hr = this->constantBuffer.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "컨스탠트 버퍼 생성에 실패했습니다");
		return false;
	}

	camera.SetPosition(0.0f, 0.0f, -2.0f);
	camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}
