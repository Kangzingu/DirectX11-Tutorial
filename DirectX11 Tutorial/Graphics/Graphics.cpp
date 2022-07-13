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
	this->cb_ps_light.ApplyChanges();
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_light.GetAddressOf());

	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 이거랑

	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);

	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);// 투명 쓸거면 첫번째 인자 "this->blendState.Get()"로

	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	// 그리는 순서때문에 불투명한거 다음에 그 앞에있는 투명한게 그려지면 불투명한게 안그려짐, 해결법은 불투명한거 먼저 그리고 투명한걸 소팅한담에 투명한건 뒤에서부터 앞 순서대로 그리기
	{// Draw
		this->gameObject.Draw(XMMatrixScaling(0.1f, 0.1f, 0.1f) * camera.GetViewMatrix() * camera.GetProjectionMatrix());
		this->gameObject2.Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
	}

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
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	static int counter = 0;
	// ImGui 프레임 시작
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// ImGui 테스트 윈도우 생성
	ImGui::Begin("Light Controls");
	ImGui::DragFloat3("Ambient Light Color", &this->cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &this->cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	/*ImGui::Text("This is example text");
	if (ImGui::Button("Click me"))
		counter++;
	ImGui::SameLine();
	std::string clickCount = "Click Count: " + std::to_string(counter);
	ImGui::Text(clickCount.c_str());
	ImGui::DragFloat3("Translation X/Y/Z", translationOffset, 0.1f, -5.0f, 5.0f);*/
	ImGui::End();
	// 그릴 데이터 모으기
	ImGui::Render();
	// 그리기
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapchain->Present(0, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		// 그래픽 카드 정보 읽어옴
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
		if (adapters.size() < 1)
		{
			ErrorLogger::Log("IDXGI Adapter를 찾지 못했습니다");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };

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
		COM_ERROR_IF_FAILED(hr, "디바이스와 스왑체인 생성에 실패했습니다");

		// 렌더 타겟(DirectX에서 렌더 타겟은 백버퍼를 의미하는 듯) 관련..?
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "스왑체인의 GetBuffer 함수 호출에 실패했습니다");

		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "렌더타겟 뷰 생성에 실패했습니다");

		// Depth/Stencil Buffer(z값 체크해서 젤 가까운것만 그려주는겅) 관련("OMSetRenderTargets" 전에 와야하는 듯? 근데 이거 뒤에도 있음..)
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;

		hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Depth Stencil 텍스쳐 생성에 실패했습니다");

		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Depth Stencil 뷰 생성에 실패했습니다");

		// 이제 렌더 타겟을 설정? 채우기? 만 하면 댐. 이게 Output Merger 단계를 위한 준비의 다인듯?
		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		// Depth/Stencil State 생성. 위에 depthStencilDesc랑 이름이 너무 똑같다.. 이건 꼭 바꿔주자..
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;// 일케하면 깊이가 같은 경우에는 걍 넘어가는듯, LESS_EQUAL은 같은 경우 늦게 들어온넘을 그리고

		hr = this->device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Depth Stencil 상태 생성에 실패했습니다");

		// 뷰포트 및 라스터라이저 상태 생성. 이게 Rasterizer 단계를 위한 준비의 다인듯
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
		this->deviceContext->RSSetViewports(1, &viewport);

		// 라스터라이저 상태 생성
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		//rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;//D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // 참고로 OpenGL은 ccw, DirectX는 cw임
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "라스터라이저 상태 생성에 실패했습니다");
		// 테스트용(앞면 컬링?) 라스터라이저 상태 생성
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront, this->rasterizerState_CULLFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "라스터라이저 상태 생성에 실패했습니다");

		// 블렌드 상태 생성
		D3D11_BLEND_DESC blendDesc = { 0 };
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };

		rtbd.BlendEnable = true;
		// Final = (SrcBlend * SrcBlendAlpha) + (DestBlend * DestBlendAlpha)
		// 얜 결국 = (SrcBlend * 1) + ((1 - SrcBlend) * 0) = SrcBlend
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0] = rtbd;
		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "블렌드 상태 생성에 실패했습니다");

		// 글자 관련 코드
		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data/Fonts/comic_sans_ms_16.spritefont");

		// 텍스쳐 관련 코드

		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "샘플러 상태 생성에 실패했습니다");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
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
	try
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "이미지 파일로부터 WIC 텍스쳐 생성에 실패했습니다");
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "이미지 파일로부터 WIC 텍스쳐 생성에 실패했습니다");
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/seamless_Pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "이미지 파일로부터 WIC 텍스쳐 생성에 실패했습니다");

		// Constant Buffer 초기화
		hr = this->cb_vs_vertexshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "버텍스 쉐이더의 컨스탠트 버퍼 생성에 실패했습니다");
		hr = this->cb_ps_light.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "픽셀 쉐이더의 컨스탠트 버퍼 생성에 실패했습니다");
		
		this->cb_ps_light.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
		this->cb_ps_light.data.ambientLightStrength = 1.0f;

		// 모델 초기화
		if (!gameObject.Initialize("Data/Objects/Samples/dodge_challenger.fbx", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader))
			return false;
		if (!gameObject2.Initialize("Data/Objects/nanosuit/nanosuit.obj", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader))
			return false;

		camera.SetPosition(0.0f, 0.0f, -2.0f);
		camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	gameObject.SetPosition(5.0f, 0.0f, 0.0f);
	gameObject2.SetPosition(-5.0f, 0.0f, 0.0f);

	return true;
}