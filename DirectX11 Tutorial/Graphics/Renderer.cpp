#include "Renderer.h"

/* DirectX 11 �׷��Ƚ� ���������� ����(https://www.youtube.com/watch?v=4RFlOvVPuys)*/
// 1. Input-Assembler => Not Programmable
// 2. Vertex Shader
// 3. Tesselation
// 4. Geometry Shader
// 5. Rasterizer => Not Programmable
// 6. Pixel Shader
// 7. Output-Merge => Not Programmable
// 8. Render Target

/* �츮�� ���� ��(�Ƹ���?) ���� */
// 1. Input-Assembler
// 2. Vertex Shader
// 3. Resterizer
// 4. Pixel Shader
// 5. Output-Merger

bool Renderer::Initialize(HWND hwnd, int windowWidth, int windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	timer.Start();

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
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

void Renderer::RenderFrame()
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

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �̰Ŷ�

	deviceContext->RSSetState(rasterizerState.Get());
	
	deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);// ���� ���Ÿ� ù��° ���� "blendState.Get()"��

	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());


	// 2D �׸���
	deviceContext->OMSetDepthStencilState(depthStencilState_drawMask.Get(), 0);
	deviceContext->IASetInputLayout(vertexshader_2d.GetInputLayout());
	deviceContext->VSSetShader(vertexshader_2d.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelshader_2d_discard.GetShader(), NULL, 0);
	sprite.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());



	deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);
	deviceContext->IASetInputLayout(vertexshader.GetInputLayout());
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

	// �׸��� ���������� �������Ѱ� ������ �� �տ��ִ� �����Ѱ� �׷����� �������Ѱ� �ȱ׷���, �ذ���� �������Ѱ� ���� �׸��� �����Ѱ� �����Ѵ㿡 �����Ѱ� �ڿ������� �� ������� �׸���
	{// Draw
		gameObject.Draw(XMMatrixScaling(0.1f, 0.1f, 0.1f) * camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
		gameObject2.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
	}
	{
		// �׸��� ���� ���̴� �ٲٱ�
		deviceContext->PSSetShader(pixelshader_nolight.GetShader(), NULL, 0);
		light.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
	}

	// FPS ���� ����
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
	// ImGui ������ ����
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// ImGui �׽�Ʈ ������ ����
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
	// �׸� ������ ������
	ImGui::Render();
	// �׸���
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swapchain->Present(0, NULL);
}

bool Renderer::InitializeDirectX(HWND hwnd)
{
	try
	{
		// �׷��� ī�� ���� �о��
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
		if (adapters.size() < 1)
		{
			ErrorLogger::Log("IDXGI Adapter�� ã�� ���߽��ϴ�");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };

		// backbuffer�� display mode�� ���� �ɼ�
		scd.BufferDesc.Width = windowWidth;
		scd.BufferDesc.Height = windowHeight;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// ��+�����Ϳ� �Ҵ��� ����Ʈ ��? ���õ� ���ε�
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// multi-sampling �Ķ����
		scd.SampleDesc.Count = 1;// 1�� ��Ƽ ���ø��� ���� �ʰڴٴ� ��
		scd.SampleDesc.Quality = 0;// �굵 ��������

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		scd.BufferCount = 1;// ���� ���۸�: 1, ���� ���۸�: 2

		scd.OutputWindow = hwnd;

		scd.Windowed = TRUE;

		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter,// IDXGI Adapter
										   D3D_DRIVER_TYPE_UNKNOWN,
										   NULL,// ����Ʈ���� ����̹� Ÿ��
										   NULL,// runtime layers�� ���� flags
										   NULL,// feature levels array
										   0,// array �� feature level�� #(�����ε�?)
										   D3D11_SDK_VERSION,
										   &scd,// ����ü�� description
										   swapchain.GetAddressOf(),// ����ü�� �ּ�
										   device.GetAddressOf(),// ����̽� �ּ�
										   NULL,// �����Ǵ� feature level
										   deviceContext.GetAddressOf());// ����̽� ���ؽ�Ʈ �ּ�
		COM_ERROR_IF_FAILED(hr, "����̽��� ����ü�� ������ �����߽��ϴ�");

		// ���� Ÿ��(DirectX���� ���� Ÿ���� ����۸� �ǹ��ϴ� ��) ����..?
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "����ü���� GetBuffer �Լ� ȣ�⿡ �����߽��ϴ�");

		hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "����Ÿ�� �� ������ �����߽��ϴ�");

		// Depth/Stencil Buffer(z�� üũ�ؼ� �� �����͸� �׷��ִ°�) ����("OMSetRenderTargets" ���� �;��ϴ� ��? �ٵ� �̰� �ڿ��� ����..)
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, windowWidth, windowHeight);
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;

		hr = device->CreateTexture2D(&depthStencilDesc, NULL, depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Depth Stencil �ؽ��� ������ �����߽��ϴ�");

		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Depth Stencil �� ������ �����߽��ϴ�");

		// ���� ���� Ÿ���� ����? ä���? �� �ϸ� ��. �̰� Output Merger �ܰ踦 ���� �غ��� ���ε�?
		deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		// Depth/Stencil State ����. ���� depthStencilDesc�� �̸��� �ʹ� �Ȱ���.. �̰� �� �ٲ�����..
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;// �����ϸ� ���̰� ���� ��쿡�� �� �Ѿ�µ�, LESS_EQUAL�� ���� ��� �ʰ� ���³��� �׸���
		hr = device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Depth Stencil ���� ������ �����߽��ϴ�");

		CD3D11_DEPTH_STENCIL_DESC depthstencildesc_drawMask(D3D11_DEFAULT);
		depthstencildesc_drawMask.DepthEnable = FALSE;
		depthstencildesc_drawMask.StencilEnable = TRUE;
		depthstencildesc_drawMask.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		depthstencildesc_drawMask.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_drawMask.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_drawMask.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_drawMask.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		depthstencildesc_drawMask.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_drawMask.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_drawMask.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR_SAT;
		hr = device->CreateDepthStencilState(&depthstencildesc_drawMask, depthStencilState_drawMask.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "DrawMask�� ���� Depth Stencil ���� ������ �����߽��ϴ�");
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc_applyMask(D3D11_DEFAULT);
		depthstencildesc_applyMask.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		depthstencildesc_applyMask.StencilEnable = TRUE;
		depthstencildesc_applyMask.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		depthstencildesc_applyMask.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_applyMask.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_applyMask.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_applyMask.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		depthstencildesc_applyMask.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_applyMask.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_applyMask.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		hr = device->CreateDepthStencilState(&depthstencildesc_applyMask, depthStencilState_applyMask.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "ApplyMask�� ���� Depth Stencil ���� ������ �����߽��ϴ�");

		// ����Ʈ �� ���Ͷ����� ���� ����. �̰� Rasterizer �ܰ踦 ���� �غ��� ���ε�
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));
		deviceContext->RSSetViewports(1, &viewport);

		// ���Ͷ����� ���� ����
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		//rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;//D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // ����� OpenGL�� ccw, DirectX�� cw��
		hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "���Ͷ����� ���� ������ �����߽��ϴ�");
		// �׽�Ʈ��(�ո� �ø�?) ���Ͷ����� ���� ����
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = device->CreateRasterizerState(&rasterizerDesc_CullFront, rasterizerState_CULLFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "���Ͷ����� ���� ������ �����߽��ϴ�");

		// ���� ���� ����
		D3D11_BLEND_DESC blendDesc = { 0 };
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };

		rtbd.BlendEnable = true;
		// Final = (SrcBlend * SrcBlendAlpha) + (DestBlend * DestBlendAlpha)
		// �� �ᱹ = (SrcBlend * 1) + ((1 - SrcBlend) * 0) = SrcBlend
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0] = rtbd;
		hr = device->CreateBlendState(&blendDesc, blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "���� ���� ������ �����߽��ϴ�");

		// ���� ���� �ڵ�
		spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Data/Fonts/comic_sans_ms_16.spritefont");

		// �ؽ��� ���� �ڵ�

		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "���÷� ���� ������ �����߽��ϴ�");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

bool Renderer::InitializeShaders()
{

	// ���̴��� �о�;��ϴ� �� ��������� ��ó������
	std::wstring shaderfolder = L"";
#pragma region DetemineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG // ����� ���
#ifdef _WIN64 // x64
		shaderfolder = L"../x64/Debug/";
#else // x86
		shaderfolder = L"../Debug/";
#endif
#else // ������ ���
#ifdef _WIN64 // x64
		shaderfolder = L"../x64/Release/";
#else // x86
		shaderfolder = L"../Release/";
#endif
#endif
	}

	// 2D Shaders
	D3D11_INPUT_ELEMENT_DESC layout2D[] = {
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements2D = ARRAYSIZE(layout2D);
	if (!vertexshader_2d.Initialize(device, shaderfolder + L"vertexshader_2d.cso", layout2D, numElements2D))
		return false;
	if (!pixelshader_2d.Initialize(device, shaderfolder + L"pixelshader_2d.cso"))
		return false;
	if (!pixelshader_2d_discard.Initialize(device, shaderfolder + L"pixelshader_2d_discard.cso"))
		return false;

	// 3D Shaders
	D3D11_INPUT_ELEMENT_DESC layout3D[] = {
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements3D = ARRAYSIZE(layout3D);
	if (!vertexshader.Initialize(device, shaderfolder + L"vertexshader.cso", layout3D, numElements3D))
		return false;
	if (!pixelshader.Initialize(device, shaderfolder + L"pixelshader.cso"))
		return false;
	if (!pixelshader_nolight.Initialize(device, shaderfolder + L"pixelshader_nolight.cso"))
		return false;

	return true;
}

bool Renderer::InitializeScene()
{
	try
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data/Textures/seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");
		hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data/Textures/pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");
		hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data/Textures/seamless_Pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");

		// Constant Buffer �ʱ�ȭ
		hr = cb_vs_vertexshader.Initialize(device.Get(), deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "���ؽ� ���̴��� ������Ʈ ���� ������ �����߽��ϴ�");
		hr = cb_vs_vertexshader_2d.Initialize(device.Get(), deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "���ؽ� ���̴��� 2D ������Ʈ ���� ������ �����߽��ϴ�");
		hr = cb_ps_light.Initialize(device.Get(), deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "�ȼ� ���̴��� ������Ʈ ���� ������ �����߽��ϴ�");

		cb_ps_light.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
		cb_ps_light.data.ambientLightStrength = 1.0f;

		// �� �ʱ�ȭ
		if (!gameObject.Initialize("Data/Objects/Samples/dodge_challenger.fbx", device.Get(), deviceContext.Get(), cb_vs_vertexshader))
			return false;
		if (!gameObject2.Initialize("Data/Objects/nanosuit/nanosuit.obj", device.Get(), deviceContext.Get(), cb_vs_vertexshader))
			return false;

		if (!light.Initialize(device.Get(), deviceContext.Get(), cb_vs_vertexshader))
			return false;

		if (!sprite.Initialize(device.Get(), deviceContext.Get(), 256, 256, "Data/Textures/circle.png", cb_vs_vertexshader_2d))
			return false;

		camera2D.SetProjectionValues(windowWidth, windowHeight, 0.0f, 1.0f);
		camera3D.SetPosition(0.0f, 0.0f, -2.0f);
		camera3D.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	sprite.SetPosition(XMFLOAT3(windowWidth / 2 - sprite.GetWidth()/2, windowHeight/ 2 - sprite.GetHeight() / 2, 0.0f));

	gameObject.SetPosition(5.0f, 0.0f, 0.0f);
	gameObject2.SetPosition(-5.0f, 0.0f, 0.0f);

	return true;
}