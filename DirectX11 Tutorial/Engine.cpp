#include "Engine.h"

#include <CommonStates.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <SimpleMath.h>
#include "DirectXHelpers.h"
#include <d3d11.h>
#include <wrl.h>
#include <memory>
void Engine::Initialize(HINSTANCE hInstance)
{
	InitializeWindow(hInstance);
	InitializeDirectX();
	InitializeShaders();
	InitializeScene();
}
void Engine::Run()
{
	while (IsRenderWindowExist())
	{
		HandleEvent();
		UpdatePhysics();
		UpdateTimer();
		UpdateScene();
	}
}
void Engine::InitializeWindow(HINSTANCE hInstance)
{
	this->windowManager.Initialize(hInstance, "Simple Physics Engine", "Default", 800, 600);
}
void Engine::InitializeDirectX()
{
	// ���� ü��
	vector<AdapterData> graphicsAdapter = AdapterReader::GetAdapters();
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	ComPtr<ID3D11Texture2D> swapChainBuffer;
	swapChainDescription.OutputWindow = this->windowManager.window.GetHWND();
	swapChainDescription.BufferDesc.Width = this->windowManager.window.GetWidth();
	swapChainDescription.BufferDesc.Height = this->windowManager.window.GetHeight();
	swapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// ��+�����Ϳ� �Ҵ��� ����Ʈ ��? ���õ� ���ε�
	swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDescription.SampleDesc.Count = 1;// 1�� ��Ƽ ���ø��� ���� �ʰڴٴ� ��
	swapChainDescription.SampleDesc.Quality = 0;// �굵 ��������
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.BufferCount = 1;// ���� ���۸�: 1, ���� ���۸�: 2
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	ERROR_IF_FAILED(D3D11CreateDeviceAndSwapChain(graphicsAdapter[0].pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, NULL, 0, D3D11_SDK_VERSION, &swapChainDescription, swapchain.GetAddressOf(), device.GetAddressOf(), NULL, deviceContext.GetAddressOf()), "����̽��� ����ü�� ������ �����߽��ϴ�");
	ERROR_IF_FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(swapChainBuffer.GetAddressOf())),"����ü���� GetBuffer �Լ� ȣ�⿡ �����߽��ϴ�");
	ERROR_IF_FAILED(device->CreateRenderTargetView(swapChainBuffer.Get(), NULL, renderTargetView.GetAddressOf()),"����Ÿ�� �� ������ �����߽��ϴ�");

	// ����Ʈ
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowManager.window.GetWidth()), static_cast<float>(this->windowManager.window.GetHeight()));
	deviceContext->RSSetViewports(1, &viewport);

	// ���Ͷ�����
	CD3D11_RASTERIZER_DESC rasterizerDescription(D3D11_DEFAULT);
	rasterizerDescription.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;// D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // ����� OpenGL�� ccw, DirectX�� cw��
	ERROR_IF_FAILED(device->CreateRasterizerState(&rasterizerDescription, rasterizerState.GetAddressOf()),"���Ͷ����� ���� ������ �����߽��ϴ�");

	// ���� �׽�Ʈ
	CD3D11_TEXTURE2D_DESC depthStencilTextureDescription(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowManager.window.GetWidth(), this->windowManager.window.GetHeight());
	depthStencilTextureDescription.MipLevels = 1;
	depthStencilTextureDescription.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	ERROR_IF_FAILED(device->CreateTexture2D(&depthStencilTextureDescription, NULL, depthStencilBuffer.GetAddressOf()), "Depth Stencil �ؽ��� ������ �����߽��ϴ�");
	ERROR_IF_FAILED(device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf()), "Depth Stencil �� ������ �����߽��ϴ�");
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	CD3D11_DEPTH_STENCIL_DESC depthStencilDescription(D3D11_DEFAULT);
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;// �����ϸ� ���̰� ���� ��쿡�� �� �Ѿ�µ�, LESS_EQUAL�� ���� ��� �ʰ� ���³��� �׸���
	ERROR_IF_FAILED(device->CreateDepthStencilState(&depthStencilDescription, depthStencilState.GetAddressOf()),"Depth Stencil ���� ������ �����߽��ϴ�");

	// ����
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
					"���� ���� ������ �����߽��ϴ�");

	// �ؽ���
	CD3D11_SAMPLER_DESC samplerDescription(D3D11_DEFAULT);
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ERROR_IF_FAILED(device->CreateSamplerState(&samplerDescription, samplerState.GetAddressOf()),
					"���÷� ���� ������ �����߽��ϴ�");

	// ��Ʈ
	spriteBatch = make_unique<DirectX::SpriteBatch>(deviceContext.Get());
	spriteFont = make_unique<DirectX::SpriteFont>(device.Get(), L"Assets/Fonts/NanumSquare_ac_16.spritefont");

	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(windowManager.window.GetHWND());
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	ImGui::StyleColorsDark();
}
void Engine::InitializeShaders()
{
	wstring shaderPath = L"";
#pragma region DetemineShaderPath
	if (IsDebuggerPresent())
	{
#ifdef _DEBUG// ����� ���
#ifdef _WIN64// x64
		shaderPath = L"../x64/Debug/";
#else// x86
		shaderPath = L"../Debug/";
#endif
#else// ������ ���
#ifdef _WIN64// x64
		shaderPath = L"../x64/Release/";
#else// x86
		shaderPath = L"../Release/";
#endif
#endif
	}
	D3D11_INPUT_ELEMENT_DESC inputElementDescription3D[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numofElements3D = ARRAYSIZE(inputElementDescription3D);
	vertexShader.Initialize(device, shaderPath + L"vertexshader.cso", inputElementDescription3D, numofElements3D);
	pixelShader.Initialize(device, shaderPath + L"pixelshader.cso");
	pixelShaderNoLight.Initialize(device, shaderPath + L"pixelshader_nolight.cso");
	}
void Engine::InitializeScene()
{
	// �̹��� ���� �ε�
	ERROR_IF_FAILED(DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/seamless_grass.jpg", nullptr, grassTexture.GetAddressOf()),
					"�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");
	ERROR_IF_FAILED(DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/pinksquare.jpg", nullptr, pinkTexture.GetAddressOf()),
					"�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");
	ERROR_IF_FAILED(DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/seamless_Pavement.jpg", nullptr, pavementTexture.GetAddressOf()),
					"�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");

	// Constant Buffer �ʱ�ȭ
	ERROR_IF_FAILED(vsConstantBuffer.Initialize(device.Get(), deviceContext.Get()),
					"���ؽ� ���̴��� ������Ʈ ���� ������ �����߽��ϴ�");
	ERROR_IF_FAILED(psConstantBuffer.Initialize(device.Get(), deviceContext.Get()),
					"�ȼ� ���̴��� ������Ʈ ���� ������ �����߽��ϴ�");
	psConstantBuffer.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	psConstantBuffer.data.ambientLightStrength = 0.5f;

	// ���� ������Ʈ ����
	RenderableGameObject gameObject;
	gameObject.Initialize("Assets/Objects/Cube.obj", device.Get(), deviceContext.Get(), vsConstantBuffer, aiColor3D(0.0f, 0.0f, 1.0f));
	gameObjects.push_back(gameObject);
	gameObject.Initialize("Assets/Objects/Cube.obj", device.Get(), deviceContext.Get(), vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
	gameObjects.push_back(gameObject);
	gameObjects.push_back(gameObject);
	gameObjects.push_back(gameObject);
	gameObjects[0].transform.SetScale(100.0f, 0.1f, 100.1f);
	for (int i = 0; i < gameObjects.size(); i++)
		gameObjects[i].transform.SetPosition(i * 2.0f, i * 0.3f, 0.0f);

	light.Initialize(device.Get(), deviceContext.Get(), vsConstantBuffer, aiColor3D(0.5f, 0.5f, 0.5f));

	camera.transform.SetPosition(2.0f, 3.0f, -10.0f);
	camera.SetProjectionValues(90.0f, static_cast<float>(this->windowManager.window.GetWidth()) / static_cast<float>(this->windowManager.window.GetHeight()), 0.1f, 3000.0f);

	sceneTimer.Start();
	fpsTimer.Start();
}
void Engine::HandleEvent()
{
	// ������ �޽��� ó��
	windowManager.window.HandleMessage();

	// ���콺 �Է�
	while (!windowManager.mouse.IsEventBufferEmpty())
	{
		MouseEvent me = windowManager.mouse.ReadEvent();
		//if (windowManager.mouse.IsRightDown() == true)
		if (me.GetType() == MouseEvent::Type::RAW_MOVE)
		{
			this->camera.transform.AdjustRotation((float)me.GetPosY() * 0.001f, (float)me.GetPosX() * 0.001f, 0);
		}
	}

	float cameraSpeed = 10.0f;
	// Ű���� �Է�
	while (!windowManager.keyboard.IsCharBufferEmpty())
	{
		unsigned char ch = windowManager.keyboard.ReadChar();
	}
	while (!windowManager.keyboard.IsKeyBufferEmpty())
	{
		KeyboardEvent kbe = windowManager.keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	if (windowManager.keyboard.KeyIsPressed('W'))
	{
		this->camera.transform.AdjustPosition(this->camera.transform.GetForwardVector() * cameraSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed('A'))
	{
		this->camera.transform.AdjustPosition(this->camera.transform.GetLeftVector() * cameraSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed('S'))
	{
		this->camera.transform.AdjustPosition(this->camera.transform.GetBackwardVector() * cameraSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed('D'))
	{
		this->camera.transform.AdjustPosition(this->camera.transform.GetRightVector() * cameraSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed(VK_SPACE))
	{
		this->camera.transform.AdjustPosition(0.0f, deltaTime, 0.0f);
	}
	if (windowManager.keyboard.KeyIsPressed(VK_CONTROL))
	{
		this->camera.transform.AdjustPosition(0.0f, -deltaTime, 0.0f);
	}
	// �̰� ��������..
	camera.UpdateMatrix();
}
void Engine::UpdatePhysics()
{
	// �и� �ӵ� = (a�� �ӵ� - b�� �ӵ�) * (b���� a�� ���ϴ� ����)
	// a�� ���� * a�� �ӵ� + b�� ���� * b�� �ӵ� = a�� �浹 ���� ���� * a�� �浹 ���� �ӵ� + b�� �浹 ���� ���� * b�� �浹 ���� �ӵ�
	// �浹 ���� �и� �ӵ� = -�ݹ߰�� * �и� �ӵ�

	// �η�
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (i == 0) continue;
		float d = gameObjects[i].transform.GetPositionFloat3().y;
		float waterHeight = gameObjects[0].transform.GetPositionFloat3().y;
		float maxDepth = 0.5f;
		float volumn = 1.0f;// ���� ����.. m^3 ������
		float density = 1000.0f;// ���� �е�.. kg/m^3 ������
		XMVECTOR bouyancy;
		// y=0.5	d=0
		// y=0		d=0.5
		// y=-0.5	d=1
		if (d >= waterHeight + maxDepth)
		{
			bouyancy = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else if (d <= waterHeight - maxDepth)
		{
			bouyancy = XMVectorSet(0.0f, volumn * density, 0.0f, 0.0f);
		}
		else
		{
			bouyancy = XMVectorSet(0.0f, volumn * density * (d - maxDepth - waterHeight) / (2 * maxDepth), 0.0f, 0.0f);
		}
		gameObjects[i].rigidbody.AddForce(bouyancy);
	}

	//// ������ ��(����)
	//for (int i = 1; i < gameObjects.size(); i++)
	//{
	//	XMVECTOR spring = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//	float k = 30.0f;
	//	float distAtFirst = sqrt(25.0f);
	//	if (XMVectorGetX(XMVector3Length(gameObjects[i].GetPositionVector() - gameObjects[0].GetPositionVector())) < distAtFirst) continue;
	//	spring += -k * XMVector3Normalize(gameObjects[i].GetPositionVector() - gameObjects[0].GetPositionVector()) * (XMVectorGetX(XMVector3Length(gameObjects[i].GetPositionVector() - gameObjects[0].GetPositionVector())) - distAtFirst);
	//	gameObjects[i].rigidbody.AddForce(spring);
	//}

	//// ������ ��
	//for (int i = 1; i < gameObjects.size(); i++)
	//{
	//	XMVECTOR spring = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//	float k = 30.0f;
	//	float distAtFirst = sqrt(25.0f);
	//		spring += -k * XMVector3Normalize(gameObjects[i].GetPositionVector() - gameObjects[0].GetPositionVector()) * (XMVectorGetX(XMVector3Length(gameObjects[i].GetPositionVector() - gameObjects[0].GetPositionVector())) - distAtFirst);
	//	gameObjects[i].rigidbody.AddForce(spring);
	//}

	// �׷�
	for (int i = 0; i < gameObjects.size(); i++)
	{
		XMVECTOR drag;
		float k1 = 10.0f, k2 = 10.0f;
		// ���� �ٸ��� �ѵ� ��ü�� ������� ���� �������װ���� ������ ����
		// ��: 0.47
		// ť��: 1.05
		// �Ǹ���: 0.82
		float velocityMagnitude = XMVectorGetX(XMVector3Length(gameObjects[i].rigidbody.velocity));
		drag = -XMVector3Normalize(gameObjects[i].rigidbody.velocity) * (k1 * velocityMagnitude + k2 * velocityMagnitude * velocityMagnitude);
		gameObjects[i].rigidbody.AddForce(drag);
	}

	// �߷�
	for (int i = 0; i < gameObjects.size(); i++)
	{
		XMVECTOR gravity = XMVectorSet(0.0f, -9.8f, 0.0f, 0.0f);
		gameObjects[i].rigidbody.AddForce(gravity * gameObjects[i].rigidbody.mass);
		// �߷��� ���� ������°Ŷ� ���� ������� �����ѵ�..? �׷��� ������ ������ �ݿ����༭ �ᱹ�� ������ ������ �ȹް� ����
	}

	static bool isDirUp = false;
	gameObjects[0].rigidbody.accumulatedForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	if (gameObjects[0].transform.GetPositionFloat3().y > 1.0f)
		isDirUp = false;
	else if (gameObjects[0].transform.GetPositionFloat3().y < -1.0f)
		isDirUp = true;
	if (isDirUp == true)
		gameObjects[0].transform.AdjustPosition(0, 0.0002f, 0);
	else
		gameObjects[0].transform.AdjustPosition(0, -0.0002f, 0);

	// ���� �� ��ġ�� ������Ʈ
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i].rigidbody.isKinematic)
			continue;
		/* pow����� ���ϱ� ���� �׳� damping���� 1�� ������ �ΰ� pow(damping, deltaTime) ��� damping�� �״�� �����ֱ⵵ �� */
		gameObjects[i].rigidbody.velocity += (gameObjects[i].rigidbody.accumulatedForce / gameObjects[i].rigidbody.mass) * deltaTime;
		gameObjects[i].rigidbody.velocity *= pow(gameObjects[i].rigidbody.damping, deltaTime);
		gameObjects[i].transform.AdjustPosition(gameObjects[i].rigidbody.velocity * deltaTime);
		gameObjects[i].rigidbody.accumulatedForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	}
}
void Engine::UpdateTimer()
{
	fpsCount += 1;
	if (fpsTimer.GetElapsedMiliseconds() > 1000.0f)
	{
		fps = "FPS: " + to_string(fpsCount);
		fpsCount = 0;
		fpsTimer.Restart();
	}
	deltaTime = sceneTimer.GetElapsedMiliseconds() / 1000.0f;
	sceneTimer.Restart();
}
void Engine::UpdateScene()
{
	float backgroundColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), backgroundColor);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	deviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	psConstantBuffer.data.dynamicLightColor = light.lightColor;
	psConstantBuffer.data.dynamicLightStrength = light.lightStrength;
	psConstantBuffer.data.dynamicLightPosition = light.transform.GetPositionFloat3();
	psConstantBuffer.data.dynamicLightAttenuation_a = light.attenuation_a;
	psConstantBuffer.data.dynamicLightAttenuation_b = light.attenuation_b;
	psConstantBuffer.data.dynamicLightAttenuation_c = light.attenuation_c;
	psConstantBuffer.ApplyChanges();
	deviceContext->PSSetConstantBuffers(0, 1, psConstantBuffer.GetAddressOf());
	deviceContext->RSSetState(rasterizerState.Get());
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);// ���� ���Ÿ� ù��° ���� "blendState.Get()"��

	{// ������Ʈ �׸���
		for (int i = 0; i < gameObjects.size(); i++)
		{
			gameObjects[i].Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
		}
		//gameObject.Draw(XMMatrixScaling(0.1f, 0.1f, 0.1f) * camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
		//gameObject2.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());	
		deviceContext->PSSetShader(pixelShaderNoLight.GetShader(), NULL, 0);
		light.Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
	}
	{// �� �׸���
		//std::unique_ptr<DirectX::CommonStates> m_states;
		//m_states = std::make_unique<CommonStates>(device.Get());
		//deviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
		unique_ptr<BasicEffect> basicEffect;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> primitiveBatchInputLayout;
		basicEffect = make_unique<BasicEffect>(device.Get());
		basicEffect->SetVertexColorEnabled(true);
		basicEffect->SetMatrices(SimpleMath::Matrix::Identity, camera.GetViewMatrix(), camera.GetProjectionMatrix());
		basicEffect->Apply(deviceContext.Get());

		CreateInputLayoutFromEffect<VertexPositionColor>(device.Get(), basicEffect.get(), primitiveBatchInputLayout.ReleaseAndGetAddressOf());
		deviceContext->IASetInputLayout(primitiveBatchInputLayout.Get());

		unique_ptr<PrimitiveBatch<VertexPositionColor>> primitiveBatch;
		primitiveBatch = make_unique<PrimitiveBatch<VertexPositionColor>>(deviceContext.Get());
		primitiveBatch->Begin();

		XMVECTORF32 lineColor;
		XMFLOAT3 waterSurface = gameObjects[0].transform.GetPositionFloat3();
		XMVECTOR waterSurfaceVector;
		XMVECTOR halfHeight;
		for (int i = 1; i < gameObjects.size(); i++)
		{
			waterSurface.x = gameObjects[i].transform.GetPositionFloat3().x;
			waterSurface.z = gameObjects[i].transform.GetPositionFloat3().z;
			waterSurfaceVector = XMLoadFloat3(&waterSurface);
			VertexPositionColor v1(waterSurfaceVector, DirectX::Colors::White);
			float lineLength = XMVectorGetX(XMVector3Length(waterSurfaceVector - gameObjects[i].transform.GetPositionVector()));
			lineColor = { 1.0f / lineLength, 1.0f / lineLength, 1.0f, 1.0f };
			halfHeight = XMVectorSet(0.0f, gameObjects[i].transform.GetScaleFloat3().y, 0.0f, 0.0f);
			VertexPositionColor v2(gameObjects[i].transform.GetPositionVector(), lineColor);
			primitiveBatch->DrawLine(v1, v2);
		}
		primitiveBatch->End();
	}

	UpdateUI();

	swapchain->Present(0, NULL);
}
void Engine::UpdateUI()
{
	//string firstGameobjectVelocity = to_string(XMVectorGetX(gameObjects[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetY(gameObjects[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetZ(gameObjects[0].rigidbody.velocity));
	//spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(firstGameobjectVelocity).c_str(), DirectX::XMFLOAT2(0, 100), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fps).c_str(), XMFLOAT2(5, 5), DirectX::Colors::White, 0.0f, XMFLOAT2(0, 0), XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	/*
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Light Controls");
	ImGui::DragFloat3("Ambient Light Color", &psConstantBuffer.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &psConstantBuffer.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::NewLine();
	ImGui::DragFloat3("Dynamic Light Color", &light.lightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Dynamic Light Strength", &light.lightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation A", &light.attenuation_a, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation B", &light.attenuation_b, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation C", &light.attenuation_c, 0.01f, 0.0f, 10.0f);
	//ImGui::Text("This is example text");
	//if (ImGui::Button("Click me"))
	//	counter++;
	//ImGui::SameLine();
	//string clickCount = "Click Count: " + to_string(counter);
	//ImGui::Text(clickCount.c_str());
	//ImGui::DragFloat3("Translation X/Y/Z", translationOffset, 0.1f, -5.0f, 5.0f);
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());*/
}
bool Engine::IsRenderWindowExist()
{
	return windowManager.window.IsEnable();
}

class ParticleContact
{
	// 113pg ���� ����
public:
	RenderableGameObject* gameObjects[2];
	float restitution;// �ݹ߰��(�浹 ������ ����)
	XMVECTOR contactNormal;// gameObject[0]�� gameObject[1]�� �ٰ�����
	float penetration;// �浹 ����(������ ���ؾ���..��)
protected:
	void Resolve(float duration)
	{
		ResolveVelocity(duration);
	}
	float CalculateSeparatingVelocity() const
	{
		XMVECTOR relativeVelocity = gameObjects[0]->rigidbody.velocity;
		if (gameObjects[1] != nullptr)
			relativeVelocity -= gameObjects[1]->rigidbody.velocity;
		return XMVectorGetX(XMVector3Dot(relativeVelocity, contactNormal));
	}

private:
	void ResolveVelocity(float duration)
	{
		// �� ��ü�� ���ӷ�(���˼ӷ�)�� ����
		float separatingVelocity = CalculateSeparatingVelocity();

		if (separatingVelocity > 0)// ������ �ƴ϶� �־����� �ִ°Ŷ��
			return;

		// �� ��ݷ��� ����(�浹 �� ��� - �浹 �� ���)
		float newSepVelocity = -separatingVelocity * restitution;
		float deltaVelocity = newSepVelocity - separatingVelocity;

		float totalInverseMass = 1.0f /gameObjects[0]->rigidbody.mass;
		if (gameObjects[1] != nullptr) totalInverseMass += 1.0f / gameObjects[1]->rigidbody.mass;

		if (totalInverseMass <= 0) return;

		// �����ϸ� (��� ��ü ���� / �� ������ ��)���� ������ ����(������ ũ�� ��ݿ� ���� �ӵ� ��ȭ�� �۾����� �ϹǷ�)
		float impulse = deltaVelocity / totalInverseMass;

		XMVECTOR impulsePerIMess = contactNormal * impulse;

		gameObjects[0]->rigidbody.velocity = gameObjects[0]->rigidbody.velocity + impulsePerIMess / gameObjects[0]->rigidbody.mass;

		if (gameObjects[1] != nullptr)
		{
			gameObjects[1]->rigidbody.velocity=gameObjects[1]->rigidbody.velocity + impulsePerIMess / -gameObjects[1]->rigidbody.mass;
		}
	}
};