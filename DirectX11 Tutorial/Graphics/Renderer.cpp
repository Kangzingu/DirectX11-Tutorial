#include "Renderer.h"

bool Renderer::Initialize(HWND hwnd, int windowWidth, int windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	timer.Start();
	InitializeDirectX(hwnd);
	InitializeShaders();

	if (!InitializeScene())
		return false;

	// Setup ImGui

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

void Renderer::Render()
{
	cb_ps_light.data.dynamicLightColor = light.lightColor;
	cb_ps_light.data.dynamicLightStrength = light.lightStrength;
	cb_ps_light.data.dynamicLightPosition = light.GetPositionFloat3();
	cb_ps_light.data.dynamicLightAttenuation_a = light.attenuation_a;
	cb_ps_light.data.dynamicLightAttenuation_b = light.attenuation_b;
	cb_ps_light.data.dynamicLightAttenuation_c = light.attenuation_c;
	cb_ps_light.ApplyChanges();
	deviceContext->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());

	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->IASetInputLayout(vertexshader.GetInputLayout());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 이거랑

	deviceContext->RSSetState(rasterizerState.Get());
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

	deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);// 투명 쓸거면 첫번째 인자 "blendState.Get()"로

	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	
	deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	{// Draw
		gameObject.Draw(XMMatrixScaling(0.1f, 0.1f, 0.1f) * camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
		gameObject2.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
	}
	{
		// 그리기 전에 쉐이더 바꾸깅
		deviceContext->PSSetShader(pixelshader_nolight.GetShader(), NULL, 0);
		light.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
	}

	// FPS 세서 띄우기
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (timer.GetElapsedMiliseconds() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		timer.Restart();
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
	ImGui::DragFloat3("Ambient Light Color", &cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);

	ImGui::NewLine();
	ImGui::DragFloat3("Dynamic Light Color", &light.lightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Dynamic Light Strength", &light.lightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation A", &light.attenuation_a, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation B", &light.attenuation_b, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation C", &light.attenuation_c, 0.01f, 0.0f, 10.0f);
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

	swapchain->Present(0, NULL);
}

void Renderer::InitializeDirectX(HWND hwnd)
{
	// 그래픽 카드
	std::vector<AdapterData> graphicsAdapter = AdapterReader::GetAdapters();

	// 스왑 체인
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	swapChainDescription.OutputWindow = hwnd;
	swapChainDescription.BufferDesc.Width = windowWidth;
	swapChainDescription.BufferDesc.Height = windowHeight;
	swapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// 색+데이터에 할당할 바이트 수? 관련된 것인듯
	swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDescription.SampleDesc.Count = 1;// 1은 멀티 샘플링을 하지 않겠다는 것
	swapChainDescription.SampleDesc.Quality = 0;// 얘도 마찬가지
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.BufferCount = 1;// 이중 버퍼링: 1, 삼중 버퍼링: 2
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	ERROR_IF_FAILED(D3D11CreateDeviceAndSwapChain(graphicsAdapter[0].pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, NULL, 0, D3D11_SDK_VERSION, &swapChainDescription, swapchain.GetAddressOf(), device.GetAddressOf(), NULL, deviceContext.GetAddressOf()),
						"디바이스와 스왑체인 생성에 실패했습니다");
	Microsoft::WRL::ComPtr<ID3D11Texture2D> swapChainBuffer;
	ERROR_IF_FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(swapChainBuffer.GetAddressOf())),
						"스왑체인의 GetBuffer 함수 호출에 실패했습니다");
	ERROR_IF_FAILED(device->CreateRenderTargetView(swapChainBuffer.Get(), NULL, renderTargetView.GetAddressOf()),
						"렌더타겟 뷰 생성에 실패했습니다");

	// 뷰포트
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));
	deviceContext->RSSetViewports(1, &viewport);

	// 라스터라이저
	CD3D11_RASTERIZER_DESC rasterizerDescription(D3D11_DEFAULT);
	rasterizerDescription.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;// D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // 참고로 OpenGL은 ccw, DirectX는 cw임
	ERROR_IF_FAILED(device->CreateRasterizerState(&rasterizerDescription, rasterizerState.GetAddressOf()),
						"라스터라이저 상태 생성에 실패했습니다");
	
	// 깊이 테스트
	CD3D11_TEXTURE2D_DESC depthStencilTextureDescription(DXGI_FORMAT_D24_UNORM_S8_UINT, windowWidth, windowHeight);
	depthStencilTextureDescription.MipLevels = 1;
	depthStencilTextureDescription.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	ERROR_IF_FAILED(device->CreateTexture2D(&depthStencilTextureDescription, NULL, depthStencilBuffer.GetAddressOf()),
						"Depth Stencil 텍스쳐 생성에 실패했습니다");
	ERROR_IF_FAILED(device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf()),
						"Depth Stencil 뷰 생성에 실패했습니다");
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	CD3D11_DEPTH_STENCIL_DESC depthStencilDescription(D3D11_DEFAULT);
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;// 일케하면 깊이가 같은 경우에는 걍 넘어가는듯, LESS_EQUAL은 같은 경우 늦게 들어온넘을 그리고
	ERROR_IF_FAILED(device->CreateDepthStencilState(&depthStencilDescription, depthStencilState.GetAddressOf()),
						"Depth Stencil 상태 생성에 실패했습니다");

	// 블렌드
	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDescription = { 0 };
	renderTargetBlendDescription.BlendEnable = true;
	renderTargetBlendDescription.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDescription.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	renderTargetBlendDescription.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	renderTargetBlendDescription.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	renderTargetBlendDescription.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	renderTargetBlendDescription.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	renderTargetBlendDescription.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
	D3D11_BLEND_DESC blendDescription = { 0 };
	blendDescription.RenderTarget[0] = renderTargetBlendDescription;
	ERROR_IF_FAILED(device->CreateBlendState(&blendDescription, blendState.GetAddressOf()),
						"블렌드 상태 생성에 실패했습니다");
	
	// 텍스쳐
	CD3D11_SAMPLER_DESC samplerDescription(D3D11_DEFAULT);
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ERROR_IF_FAILED(device->CreateSamplerState(&samplerDescription, samplerState.GetAddressOf()),
						"샘플러 상태 생성에 실패했습니다");

	// 폰트
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Assets/Fonts/comic_sans_ms_16.spritefont");
}

void Renderer::InitializeShaders()
{
	std::wstring shaderPath = L"";
#pragma region DetemineShaderPath
	if (IsDebuggerPresent())
	{
#ifdef _DEBUG// 디버그 모드
#ifdef _WIN64// x64
		shaderPath = L"../x64/Debug/";
#else// x86
		shaderPath = L"../Debug/";
#endif
#else// 릴리즈 모드
#ifdef _WIN64// x64
		shaderPath = L"../x64/Release/";
#else// x86
		shaderPath = L"../Release/";
#endif
#endif
	}
	D3D11_INPUT_ELEMENT_DESC inputElementDescription3D[] = {
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numofElements3D = ARRAYSIZE(inputElementDescription3D);
	vertexshader.Initialize(device, shaderPath + L"vertexshader.cso", inputElementDescription3D, numofElements3D);
	pixelshader.Initialize(device, shaderPath + L"pixelshader.cso");
	pixelshader_nolight.Initialize(device, shaderPath + L"pixelshader_nolight.cso");
}

bool Renderer::InitializeScene()
{
	ERROR_IF_FAILED(DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/seamless_grass.jpg", nullptr, grassTexture.GetAddressOf()),
						"이미지 파일로부터 WIC 텍스쳐 생성에 실패했습니다");
	ERROR_IF_FAILED(DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/pinksquare.jpg", nullptr, pinkTexture.GetAddressOf()),
						"이미지 파일로부터 WIC 텍스쳐 생성에 실패했습니다");
	ERROR_IF_FAILED(DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/seamless_Pavement.jpg", nullptr, pavementTexture.GetAddressOf()),
						"이미지 파일로부터 WIC 텍스쳐 생성에 실패했습니다");

	// Constant Buffer 초기화
	ERROR_IF_FAILED(cb_vs_vertexshader.Initialize(device.Get(), deviceContext.Get()),
						"버텍스 쉐이더의 컨스탠트 버퍼 생성에 실패했습니다");
	ERROR_IF_FAILED(cb_ps_light.Initialize(device.Get(), deviceContext.Get()),
						"픽셀 쉐이더의 컨스탠트 버퍼 생성에 실패했습니다");

	cb_ps_light.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	cb_ps_light.data.ambientLightStrength = 1.0f;

	// 모델 초기화
	gameObject.Initialize("Assets/Objects/Samples/dodge_challenger.fbx", device.Get(), deviceContext.Get(), cb_vs_vertexshader);
	gameObject2.Initialize("Assets/Objects/nanosuit/nanosuit.obj", device.Get(), deviceContext.Get(), cb_vs_vertexshader);
	light.Initialize(device.Get(), deviceContext.Get(), cb_vs_vertexshader);

	camera3D.SetPosition(0.0f, 0.0f, -2.0f);
	camera3D.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);

	gameObject.SetPosition(5.0f, 0.0f, 0.0f);
	gameObject2.SetPosition(-5.0f, 0.0f, 0.0f);

	return true;
}