#include "Graphics.h"

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
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �̰Ŷ�

	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);

	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	UINT offset = 0;
	
	// Constant Buffer ������Ʈ
	XMMATRIX world = XMMatrixIdentity();
	constantBuffer.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
	if (!constantBuffer.ApplyChanges())
		return;
	this->deviceContext->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());

	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	// �ε��� ���� ���������� DWORD(32��Ʈ)�ϱ� R32(R�� RGB�̰Ŵϱ� �� �ǹ� ���°� ���� 32��Ʈ¥�� 1���� ���ϴ°� �̰Ŷ� �̰ɷ� �ѰŰ�ƴ)�� �ѵ�
	this->deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0); // �̰� �ٲٸ� �ٸ��� �׷���

	// FPS ���� ����
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

	// ImGui ������ ����
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// ImGui �׽�Ʈ ������ ����
	ImGui::Begin("Test");
	ImGui::End();
	// �׸� ������ ������
	ImGui::Render();
	// �׸���
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapchain->Present(0, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	// �׷��� ī�� ���� �о��

	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log("IDXGI Adapter�� ã�� ���߽��ϴ�");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

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
									   this->swapchain.GetAddressOf(),// ����ü�� �ּ�
									   this->device.GetAddressOf(),// ����̽� �ּ�
									   NULL,// �����Ǵ� feature level
									   this->deviceContext.GetAddressOf());// ����̽� ���ؽ�Ʈ �ּ�
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "����̽��� ����ü�� ������ �����߽��ϴ�");
		return false;
	}

	// ���� Ÿ��(DirectX���� ���� Ÿ���� ����۸� �ǹ��ϴ� ��) ����..?
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "����ü���� GetBuffer �Լ� ȣ�⿡ �����߽��ϴ�");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "����Ÿ�� �� ������ �����߽��ϴ�");
		return false;
	}

	// Depth/Stencil Buffer(z�� üũ�ؼ� �� �����͸� �׷��ִ°�) ����("OMSetRenderTargets" ���� �;��ϴ� ��? �ٵ� �̰� �ڿ��� ����..)
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
		ErrorLogger::Log(hr, "Depth Stencil �ؽ��� ������ �����߽��ϴ�");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Depth Stencil �� ������ �����߽��ϴ�");
		return false;
	}

	// ���� ���� Ÿ���� ����? ä���? �� �ϸ� ��. �̰� Output Merger �ܰ踦 ���� �غ��� ���ε�?
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	// Depth/Stencil State ����. ���� depthStencilDesc�� �̸��� �ʹ� �Ȱ���.. �̰� �� �ٲ�����..
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;//D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;// �����ϸ� ���ٽ� ����
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;// �����ϸ� ���̰� ���� ��쿡�� �� �Ѿ�µ�, LESS_EQUAL�� ���� ��� �ʰ� ���³��� �׸���

	hr = this->device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Depth Stencil ���� ������ �����߽��ϴ�");
		return false;
	}

	// ����Ʈ �� ���Ͷ����� ���� ����. �̰� Rasterizer �ܰ踦 ���� �غ��� ���ε�
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = windowWidth;
	viewport.Height = windowHeight;
	viewport.MinDepth = 0.0f; // ī�޶� ����
	viewport.MaxDepth = 1.0f; // ī�޶� ��

	this->deviceContext->RSSetViewports(1, &viewport);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;//D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;//D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // ����� OpenGL�� ccw, DirectX�� cw��
	//rasterizerDesc.FrontCounterClockwise = true;

	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "���Ͷ����� ���� ������ �����߽��ϴ�");
		return false;
	}

	// ���� ���� �ڵ�
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data/Fonts/comic_sans_ms_16.spritefont");

	// �ؽ��� ���� �ڵ�

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	// ���� LoD �����ϸ� �ּҺ��� �ִ���� �� ���� �� Ŀ���Ǵµ�
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "���÷� ���� ������ �����߽��ϴ�");
		return false;
	}

	return true;
}

bool Graphics::InitializeShaders()
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
		ErrorLogger::Log(hr, "���ؽ� ���� ������ �����߽��ϴ�");
		return false;
	}
	DWORD indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	hr = this->indicesBuffer.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "�ε��� ���� ������ �����߽��ϴ�");
		return hr;
	}

	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/TestTexture.png", nullptr, myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");
		return false;
	}

	// Constant Buffer �ʱ�ȭ
	hr = this->constantBuffer.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "������Ʈ ���� ������ �����߽��ϴ�");
		return false;
	}

	camera.SetPosition(0.0f, 0.0f, -2.0f);
	camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}
