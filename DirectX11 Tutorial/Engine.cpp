#include "Engine.h"

void Engine::Initialize(HINSTANCE hInstance)
{
	InitializeWindow(hInstance);
	InitializeRenderer();
}
void Engine::InitializeWindow(HINSTANCE hInstance)
{
	windowWidth = 800;
	windowHeight = 600;
	windowManager.Initialize(hInstance, "Simple Physics Engine", "Default", windowWidth, windowHeight);
}
void Engine::InitializeRenderer()
{
	InitializeDirectX();
	InitializeShaders();
	InitializeScene();
	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(windowManager.window.GetHWND());
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	ImGui::StyleColorsDark();
}

void Engine::Run()
{
	while (IsWindowEnable())
	{
		windowManager.window.ProcessMessages();
		ProcessDeviceInput();
		Render();
	}
}

void Engine::InitializeScene()
{
	sceneTimer.Start();
	timer.Start();

	ERROR_IF_FAILED(DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/seamless_grass.jpg", nullptr, grassTexture.GetAddressOf()),
					"�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");
	ERROR_IF_FAILED(DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/pinksquare.jpg", nullptr, pinkTexture.GetAddressOf()),
					"�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");
	ERROR_IF_FAILED(DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/seamless_Pavement.jpg", nullptr, pavementTexture.GetAddressOf()),
					"�̹��� ���Ϸκ��� WIC �ؽ��� ������ �����߽��ϴ�");

	// Constant Buffer �ʱ�ȭ
	ERROR_IF_FAILED(cb_vs_vertexshader.Initialize(device.Get(), deviceContext.Get()),
					"���ؽ� ���̴��� ������Ʈ ���� ������ �����߽��ϴ�");
	ERROR_IF_FAILED(cb_ps_light.Initialize(device.Get(), deviceContext.Get()),
					"�ȼ� ���̴��� ������Ʈ ���� ������ �����߽��ϴ�");

	cb_ps_light.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	cb_ps_light.data.ambientLightStrength = 1.0f;

	// �� �ʱ�ȭ
	gameObject.Initialize("Assets/Objects/Cube.obj", device.Get(), deviceContext.Get(), cb_vs_vertexshader, aiColor3D(0.0f, 0.0f, 1.0f));
	//gameObject2.Initialize("Assets/Objects/nanosuit/nanosuit.obj", device.Get(), deviceContext.Get(), cb_vs_vertexshader, aiColor3D(0.0f, 0.0f, 1.0f));
	gameObjects.push_back(gameObject);
	gameObject.Initialize("Assets/Objects/Cube.obj", device.Get(), deviceContext.Get(), cb_vs_vertexshader, aiColor3D(1.0f, 1.0f, 1.0f));
	gameObjects.push_back(gameObject);
	gameObjects.push_back(gameObject);
	gameObjects.push_back(gameObject);
	//gameObjects.push_back(gameObject2);
	gameObjects[0].SetScale(100.0f, 0.1f, 100.0f);

	light.Initialize(device.Get(), deviceContext.Get(), cb_vs_vertexshader, aiColor3D(0.0f, 0.0f, 1.0f));

	camera3D.SetPosition(0.0f, 0.0f, -2.0f);
	camera3D.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);

	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i].SetPosition(i * 3.0f, 0.0f, i * 3.0f);
	}

}

void Engine::Render()
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
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �̰Ŷ�

	deviceContext->RSSetState(rasterizerState.Get());
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

	deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);// ���� ���Ÿ� ù��° ���� "blendState.Get()"��

	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);
	{
		UpdatePhysics();
	}

	{// Draw
		for (int i = 0; i < gameObjects.size(); i++)
		{
			gameObjects[i].Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
		}
		//gameObject.Draw(XMMatrixScaling(0.1f, 0.1f, 0.1f) * camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
		//gameObject2.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
	}
	{
		// �׸��� ���� ���̴� �ٲٱ�
		deviceContext->PSSetShader(pixelshader_nolight.GetShader(), NULL, 0);
		light.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
	}

	deltaTime = sceneTimer.GetElapsedMiliseconds() / 1000.0f;
	sceneTimer.Restart();

	// FPS ���� ����
	static int fpsCounter = 0;
	static string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (timer.GetElapsedMiliseconds() > 1000.0)
	{
		fpsString = "FPS: " + to_string(fpsCounter);
		fpsCounter = 0;
		timer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
	string firstGameobjectVelocity = to_string(XMVectorGetX(gameObjects[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetY(gameObjects[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetZ(gameObjects[0].rigidbody.velocity));
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(firstGameobjectVelocity).c_str(), DirectX::XMFLOAT2(0, 100), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
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
	string clickCount = "Click Count: " + to_string(counter);
	ImGui::Text(clickCount.c_str());
	ImGui::DragFloat3("Translation X/Y/Z", translationOffset, 0.1f, -5.0f, 5.0f);*/
	ImGui::End();
	// �׸� ������ ������
	ImGui::Render();
	// �׸���
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swapchain->Present(0, NULL);
}

void Engine::UpdatePhysics()
{
	// å 96pg ���� ����

	//// �η�
	//for (int i = 0; i < gameObjects.size(); i++)
	//{
	//	if (i == 0) continue;
	//	float d = gameObjects[i].GetPositionFloat3().y;
	//	float waterHeight = gameObjects[0].GetPositionFloat3().y;
	//	float maxDepth = 0.5f;
	//	float volumn = 1.0f;// ���� ����.. m^3 ������
	//	float density = 1000.0f;// ���� �е�.. kg/m^3 ������
	//	XMVECTOR bouyancy;
	//	// y=0.5	d=0
	//	// y=0		d=0.5
	//	// y=-0.5	d=1
	//	if (d >= waterHeight + maxDepth)
	//	{
	//		bouyancy = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//	}
	//	else if (d <= waterHeight - maxDepth)
	//	{
	//		bouyancy = XMVectorSet(0.0f, volumn * density, 0.0f, 0.0f); 
	//	}
	//	else
	//	{
	//		bouyancy = XMVectorSet(0.0f, volumn * density * (d - maxDepth - waterHeight) / (2 * maxDepth), 0.0f, 0.0f);
	//	}
	//	gameObjects[i].rigidbody.AddForce(bouyancy);
	//}

	//// ������ ��(����)
	//for (int i = 0; i < gameObjects.size(); i++)
	//{
	//	if (i <= 2) continue;
	//	XMVECTOR spring;
	//	float k = 50.0f;ss
	//	float distAtFirst = sqrt(400.0f);
	//	if (XMVectorGetX(XMVector3Length(gameObjects[i].GetPositionVector() - gameObjects[gameObjects.size() - i - 1].GetPositionVector())) < distAtFirst) continue;
	//	spring = -k * XMVector3Normalize(gameObjects[i].GetPositionVector() - gameObjects[gameObjects.size() - i - 1].GetPositionVector()) * (XMVectorGetX(XMVector3Length(gameObjects[i].GetPositionVector() - gameObjects[gameObjects.size() - i - 1].GetPositionVector())) - distAtFirst);
	//	gameObjects[i].rigidbody.AddForce(spring);
	//}

	// ������ ��
	for (int i = 1; i < gameObjects.size(); i++)
	{
		XMVECTOR spring = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		float k = 5.0f;
		float distAtFirst = sqrt(100.0f);
		for (int j = 0; j < gameObjects.size(); j++)
		{
			spring += -k * XMVector3Normalize(gameObjects[i].GetPositionVector() - gameObjects[j].GetPositionVector()) * (XMVectorGetX(XMVector3Length(gameObjects[i].GetPositionVector() - gameObjects[j].GetPositionVector())) - distAtFirst);
		}
		gameObjects[i].rigidbody.AddForce(spring);
	}

	//// �׷�
	//for (int i = 0; i < gameObjects.size(); i++)
	//{
	//	XMVECTOR drag;
	//	float k1 = 0.01f, k2 = 0.01f;
	//	// ���� �ٸ��� �ѵ� ��ü�� ������� ���� �������װ���� ������ ����
	//	// ��: 0.47
	//	// ť��: 1.05
	//	// �Ǹ���: 0.82
	//	float velocityMagnitude = XMVectorGetX(XMVector3Length(gameObjects[i].rigidbody.velocity));
	//	drag = -XMVector3Normalize(gameObjects[i].rigidbody.velocity) * (k1 * velocityMagnitude + k2 * velocityMagnitude * velocityMagnitude);
	//	gameObjects[i].rigidbody.AddForce(drag);
	//}

	// �߷�
	for (int i = 0; i < gameObjects.size(); i++)
	{
		XMVECTOR gravity = XMVectorSet(0.0f, -9.8f, 0.0f, 0.0f);
		gameObjects[i].rigidbody.AddForce(gravity * gameObjects[i].rigidbody.mass);
		// �߷��� ���� ������°Ŷ� ���� ������� �����ѵ�..? �׷��� ������ ������ �ݿ����༭ �ᱹ�� ������ ������ �ȹް� ����
	}

	static bool isDirUp = false;
	gameObjects[0].rigidbody.accumulatedForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	if (gameObjects[0].GetPositionFloat3().y > 1.0f)
		isDirUp = false;
	else if (gameObjects[0].GetPositionFloat3().y < -1.0f)
		isDirUp = true;
	if (isDirUp == true)
		gameObjects[0].AdjustPosition(0, 0.0002f, 0);
	else
		gameObjects[0].AdjustPosition(0, -0.0002f, 0);

	// ���� �� ��ġ�� ������Ʈ
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i].rigidbody.isKinematic)
			continue;
		/* pow����� ���ϱ� ���� �׳� damping���� 1�� ������ �ΰ� pow(damping, deltaTime) ��� damping�� �״�� �����ֱ⵵ �� */
		gameObjects[i].rigidbody.velocity += (gameObjects[i].rigidbody.accumulatedForce / gameObjects[i].rigidbody.mass) * deltaTime;
		gameObjects[i].rigidbody.velocity *= pow(gameObjects[i].rigidbody.damping, deltaTime);
		gameObjects[i].AdjustPosition(gameObjects[i].rigidbody.velocity * deltaTime);
		gameObjects[i].rigidbody.accumulatedForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	}
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
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numofElements3D = ARRAYSIZE(inputElementDescription3D);
	vertexshader.Initialize(device, shaderPath + L"vertexshader.cso", inputElementDescription3D, numofElements3D);
	pixelshader.Initialize(device, shaderPath + L"pixelshader.cso");
	pixelshader_nolight.Initialize(device, shaderPath + L"pixelshader_nolight.cso");
}
void Engine::InitializeDirectX()
{
	HWND hwnd = windowManager.window.GetHWND();
	// �׷��� ī��
	vector<AdapterData> graphicsAdapter = AdapterReader::GetAdapters();

	// ���� ü��
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	swapChainDescription.OutputWindow = hwnd;
	swapChainDescription.BufferDesc.Width = windowWidth;
	swapChainDescription.BufferDesc.Height = windowHeight;
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
	ERROR_IF_FAILED(D3D11CreateDeviceAndSwapChain(graphicsAdapter[0].pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, NULL, 0, D3D11_SDK_VERSION, &swapChainDescription, swapchain.GetAddressOf(), device.GetAddressOf(), NULL, deviceContext.GetAddressOf()),
					"����̽��� ����ü�� ������ �����߽��ϴ�");
	ComPtr<ID3D11Texture2D> swapChainBuffer;
	ERROR_IF_FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(swapChainBuffer.GetAddressOf())),
					"����ü���� GetBuffer �Լ� ȣ�⿡ �����߽��ϴ�");
	ERROR_IF_FAILED(device->CreateRenderTargetView(swapChainBuffer.Get(), NULL, renderTargetView.GetAddressOf()),
					"����Ÿ�� �� ������ �����߽��ϴ�");

	// ����Ʈ
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));
	deviceContext->RSSetViewports(1, &viewport);

	// ���Ͷ�����
	CD3D11_RASTERIZER_DESC rasterizerDescription(D3D11_DEFAULT);
	rasterizerDescription.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;// D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // ����� OpenGL�� ccw, DirectX�� cw��
	ERROR_IF_FAILED(device->CreateRasterizerState(&rasterizerDescription, rasterizerState.GetAddressOf()),
					"���Ͷ����� ���� ������ �����߽��ϴ�");

	// ���� �׽�Ʈ
	CD3D11_TEXTURE2D_DESC depthStencilTextureDescription(DXGI_FORMAT_D24_UNORM_S8_UINT, windowWidth, windowHeight);
	depthStencilTextureDescription.MipLevels = 1;
	depthStencilTextureDescription.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	ERROR_IF_FAILED(device->CreateTexture2D(&depthStencilTextureDescription, NULL, depthStencilBuffer.GetAddressOf()),
					"Depth Stencil �ؽ��� ������ �����߽��ϴ�");
	ERROR_IF_FAILED(device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf()),
					"Depth Stencil �� ������ �����߽��ϴ�");
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	CD3D11_DEPTH_STENCIL_DESC depthStencilDescription(D3D11_DEFAULT);
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;// �����ϸ� ���̰� ���� ��쿡�� �� �Ѿ�µ�, LESS_EQUAL�� ���� ��� �ʰ� ���³��� �׸���
	ERROR_IF_FAILED(device->CreateDepthStencilState(&depthStencilDescription, depthStencilState.GetAddressOf()),
					"Depth Stencil ���� ������ �����߽��ϴ�");

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
	spriteFont = make_unique<DirectX::SpriteFont>(device.Get(), L"Assets/Fonts/comic_sans_ms_16.spritefont");

}
void Engine::ProcessDeviceInput()
{
	while (!windowManager.keyboard.IsCharBufferEmpty())
	{
		unsigned char ch = windowManager.keyboard.ReadChar();
	}
	while (!windowManager.keyboard.IsKeyBufferEmpty())
	{
		KeyboardEvent kbe = windowManager.keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	while (!windowManager.mouse.IsEventBufferEmpty())
	{
		MouseEvent me = windowManager.mouse.ReadEvent();
		if (windowManager.mouse.IsRightDown() == true)
		{
			if (me.GetType() == MouseEvent::Type::RAW_MOVE)
			{
				this->camera3D.AdjustRotation((float)me.GetPosY() * 0.001f, (float)me.GetPosX() * 0.001f, 0);
			}
		}
	}

	this->gameObject.AdjustRotation(0.0f, 1.0f * deltaTime, 0.0f);

	float Camera3DSpeed = 8.0f;

	if (windowManager.keyboard.KeyIsPressed(VK_SHIFT))
	{
		Camera3DSpeed = 30.0f;
	}
	if (windowManager.keyboard.KeyIsPressed('W'))
	{
		this->camera3D.AdjustPosition(this->camera3D.GetForwardVector() * Camera3DSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed('A'))
	{
		this->camera3D.AdjustPosition(this->camera3D.GetLeftVector() * Camera3DSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed('S'))
	{
		this->camera3D.AdjustPosition(this->camera3D.GetBackwardVector() * Camera3DSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed('D'))
	{
		this->camera3D.AdjustPosition(this->camera3D.GetRightVector() * Camera3DSpeed * deltaTime);
	}
	if (windowManager.keyboard.KeyIsPressed(VK_SPACE))
	{
		this->camera3D.AdjustPosition(0.0f, Camera3DSpeed * deltaTime, 0.0f);
	}
	if (windowManager.keyboard.KeyIsPressed(VK_CONTROL))
	{
		this->camera3D.AdjustPosition(0.0f, -Camera3DSpeed * deltaTime, 0.0f);
	}
	if (windowManager.keyboard.KeyIsPressed('C'))
	{
		XMVECTOR lightPosition = this->camera3D.GetPositionVector();
		lightPosition += this->camera3D.GetForwardVector();
		this->light.SetPosition(lightPosition);
		this->light.SetRotation(this->camera3D.GetRotationFloat3());
	}
}
bool Engine::IsWindowEnable()
{
	return windowManager.window.IsEnable();
}