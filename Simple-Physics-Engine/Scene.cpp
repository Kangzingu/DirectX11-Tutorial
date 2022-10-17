#include "Scene.h"

void Scene::Initialize(HINSTANCE hInstance)
{
	InitializeWindow(hInstance);
	InitializeDirectX();
	InitializeShaders();
	InitializeObjects();
	InitializePhysics();
}
void Scene::Run()
{
	while (IsRenderWindowExist())
	{
		UpdateTimer();
		HandleEvent();
		UpdatePhysics();
		CheckStopCondition();
		UpdateScene();
	}
}
void Scene::InitializeWindow(HINSTANCE hInstance)
{
	m_windowManager = new WindowManager(hInstance, "Simple Physics Engine", "Default", 1600, 900);
}
void Scene::InitializePhysics()
{
	m_physicsManager = new PhysicsManager(&m_actors, m_deltaTime, m_lineForDebug);
}
void Scene::InitializeDirectX()
{
	// 어댑터
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;
	vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* adapter;
	DXGI_ADAPTER_DESC adapterDescription;
	UINT adapterIndex = 0;
	ERROR_IF_FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf())), "DXGIFactory 생성 실패");
	while (SUCCEEDED(factory->EnumAdapters(adapterIndex, &adapter)))
	{
		ERROR_IF_FAILED(adapter->GetDesc(&adapterDescription), "AdapterData를 정상적으로 초기화하지 못했습니다(Adapter에 대한 description을 받아오지 못함)");
		adapters.push_back(adapter);
		adapterIndex += 1;
	}
	ERROR_IF(!adapters.size() > 0, "IDXGI Adapter를 찾지 못했습니다");

	// 스왑 체인
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	ComPtr<ID3D11Texture2D> swapChainBuffer;
	swapChainDescription.OutputWindow = m_windowManager->m_window.GetHWND();
	swapChainDescription.BufferDesc.Width = m_windowManager->m_window.GetWidth();
	swapChainDescription.BufferDesc.Height = m_windowManager->m_window.GetHeight();
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
	ERROR_IF_FAILED(D3D11CreateDeviceAndSwapChain(adapters[0], D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, NULL, 0, D3D11_SDK_VERSION, &swapChainDescription, m_swapchain.GetAddressOf(), m_device.GetAddressOf(), NULL, m_deviceContext.GetAddressOf()), "디바이스와 스왑체인 생성에 실패했습니다");
	ERROR_IF_FAILED(m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(swapChainBuffer.GetAddressOf())), "스왑체인의 GetBuffer 함수 호출에 실패했습니다");
	ERROR_IF_FAILED(m_device->CreateRenderTargetView(swapChainBuffer.Get(), NULL, m_renderTargetView.GetAddressOf()), "렌더타겟 뷰 생성에 실패했습니다");

	// 뷰포트
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_windowManager->m_window.GetWidth()), static_cast<float>(m_windowManager->m_window.GetHeight()));
	m_deviceContext->RSSetViewports(1, &viewport);

	// 라스터라이저
	CD3D11_RASTERIZER_DESC rasterizerDescription(D3D11_DEFAULT);
	rasterizerDescription.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;// D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // 참고로 OpenGL은 ccw, DirectX는 cw임
	ERROR_IF_FAILED(m_device->CreateRasterizerState(&rasterizerDescription, m_rasterizerState.GetAddressOf()), "라스터라이저 상태 생성에 실패했습니다");

	// 깊이 테스트
	CD3D11_TEXTURE2D_DESC depthStencilTextureDescription(DXGI_FORMAT_D24_UNORM_S8_UINT, m_windowManager->m_window.GetWidth(), m_windowManager->m_window.GetHeight());
	depthStencilTextureDescription.MipLevels = 1;
	depthStencilTextureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ERROR_IF_FAILED(m_device->CreateTexture2D(&depthStencilTextureDescription, NULL, m_depthStencilBuffer.GetAddressOf()), "Depth Stencil 텍스쳐 생성에 실패했습니다");
	ERROR_IF_FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), NULL, m_depthStencilView.GetAddressOf()), "Depth Stencil 뷰 생성에 실패했습니다");
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	CD3D11_DEPTH_STENCIL_DESC depthStencilDescription(D3D11_DEFAULT);
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;// 일케하면 깊이가 같은 경우에는 걍 넘어가는듯, LESS_EQUAL은 같은 경우 늦게 들어온넘을 그리고
	ERROR_IF_FAILED(m_device->CreateDepthStencilState(&depthStencilDescription, m_depthStencilState.GetAddressOf()), "Depth Stencil 상태 생성에 실패했습니다");

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
	ERROR_IF_FAILED(m_device->CreateBlendState(&blendDescription, m_blendState.GetAddressOf()),
					"블렌드 상태 생성에 실패했습니다");

	// 텍스쳐
	CD3D11_SAMPLER_DESC samplerDescription(D3D11_DEFAULT);
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ERROR_IF_FAILED(m_device->CreateSamplerState(&samplerDescription, m_samplerState.GetAddressOf()),
					"샘플러 상태 생성에 실패했습니다");

	// 폰트
	m_spriteBatch = make_unique<DirectX::SpriteBatch>(m_deviceContext.Get());
	m_spriteFont = make_unique<DirectX::SpriteFont>(m_device.Get(), L"Assets/Fonts/NanumSquare_ac_16.spritefont");

	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(m_windowManager->m_window.GetHWND());
	ImGui_ImplDX11_Init(m_device.Get(), m_deviceContext.Get());
	ImGui::StyleColorsDark();

	// 2D
	m_commonState = make_unique<DirectX::CommonStates>(m_device.Get());
	m_basicEffect = make_unique<DirectX::BasicEffect>(m_device.Get());
	m_basicEffect->SetVertexColorEnabled(true);
	m_primitiveBatch = make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_deviceContext.Get());
}
void Scene::InitializeShaders()
{
	wstring shaderPath = L"";
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
	D3D11_INPUT_ELEMENT_DESC inputElementDescription[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numofElements = ARRAYSIZE(inputElementDescription);
	m_vertexShader.Initialize(m_device, shaderPath + L"vertexshader.cso", inputElementDescription, numofElements);
	m_pixelShader.Initialize(m_device, shaderPath + L"pixelshader.cso");
	m_pixelShaderNoLight.Initialize(m_device, shaderPath + L"pixelshader_nolight.cso");
	m_vsConstantBuffer.Initialize(m_device.Get(), m_deviceContext.Get());
	m_psConstantBuffer.Initialize(m_device.Get(), m_deviceContext.Get());
}
void Scene::InitializeObjects()
{
	// 컴포넌트 초기화
	Actor* actor;
	Model cubeModel;
	Model triangleConeModel;
	Model quadConeModel;
	Model lightModel;
	Transform transform;
	Rigidbody rigidbody;
	Collider collider;

	// 액터
	cubeModel.Initialize("Assets/Objects/Cube.obj", m_device.Get(), m_deviceContext.Get(), m_vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
	triangleConeModel.Initialize("Assets/Objects/TriangleCone.obj", m_device.Get(), m_deviceContext.Get(), m_vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
	quadConeModel.Initialize("Assets/Objects/QuadCone.obj", m_device.Get(), m_deviceContext.Get(), m_vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
	transform.Initialize(Vector3::Zero(), Vector3::Zero(), Vector3::One());
	rigidbody.Initialize(0.1f, 0.95f, 0.95f, Vector3::Zero(), Vector3::Zero(), Matrix4x4::CubeInertiaTensor(12.0f, transform.GetScale()).Inverse());
	collider.Initialize();
	actor = new Actor();
	actor->Initialize(cubeModel, transform, rigidbody, collider);
	actor->m_transform.SetPosition(Vector3(0, 0, 0));
	actor->m_transform.SetScale(Vector3(30, 1, 30));
	actor->m_rigidbody.SetInverseMass(0);
	actor->m_rigidbody.SetKinematic(true);
	actor->m_rigidbody.SetInertiaTensorInverse(Matrix4x4::Zero());
	m_actors.push_back(actor);
	/*for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5 - i; j++)
		{
			for (int k = 0; k < 5 - i; k++)
			{
				actor = new Actor();
				actor->Initialize(model, transform, rigidbody, collider);
				actor->m_transform.SetPosition(Vector3(j - 5, i + 1, k - 5));
				m_actors.push_back(actor);
			}
		}
	}*/
	for (int i = 0; i < 30; i++)
	{
			Model cubeModel;
			cubeModel.Initialize("Assets/Objects/Cube.obj", m_device.Get(), m_deviceContext.Get(), m_vsConstantBuffer, aiColor3D(0.3f, 0.3f, 0.3f));
			actor = new Actor();
			/*if (i < 1)
				actor->Initialize(triangleConeModel, transform, rigidbody, collider);
			else if (i < 3)
				actor->Initialize(cubeModel, transform, rigidbody, collider);
			else if (i < 8)
				actor->Initialize(triangleConeModel, transform, rigidbody, collider);
			else 
				actor->Initialize(quadConeModel, transform, rigidbody, collider);*/
			actor->Initialize(cubeModel, transform, rigidbody, collider);

			actor->m_transform.SetPosition(Vector3(0, i * 2.1f + 1.1f, 0));
			//actor->m_rigidbody.SetInertiaTensorInverse(Matrix4x4::CubeInertiaTensor(12, actor->m_transform.GetScale()).Inverse());
			m_actors.push_back(actor);
	}

	// 조명
	lightModel.Initialize("Assets/Objects/Light.obj", m_device.Get(), m_deviceContext.Get(), m_vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
	transform.SetPosition(Vector3(3.0f, 10.0f, 0.0f));
	m_light = new Light();
	m_light->Initialize(lightModel, transform, rigidbody, collider);

	// 카메라
	transform.SetPosition(Vector3(0.0f, 2.0f, 3.0f));
	transform.Rotate(Vector3(General::DegreeToRadian(-15.0f), 0, 0));
	m_camera = new Camera();
	m_camera->Initialize(cubeModel, transform, rigidbody, collider);
	m_camera->SetProjectionMatrix(45.0f, static_cast<float>(m_windowManager->m_window.GetWidth()) / static_cast<float>(m_windowManager->m_window.GetHeight()), 0.1f, 3000.0f);

	// 타이머
	m_deltaTimer.Start();
	m_fpsTimer.Start();

	//m_actors[1]->m_transform.Rotate(Vector3(0, General::DegreeToRadian(45.0f), 0));
	//m_actors[2]->m_transform.Rotate(Vector3(0, 0, General::DegreeToRadian(45.0f)));
}
bool Scene::IsRenderWindowExist()
{
	return m_windowManager->m_window.IsEnable();
}
void Scene::UpdateTimer()
{
	m_fps += 1;
	if (m_fpsTimer.GetElapsedMiliseconds() > 1000.0f)
	{
		m_fpsString = "FPS: " + to_string(m_fps);
		m_fps = 0;
		m_fpsTimer.Restart();
	}
	m_deltaTime = (m_deltaTimer.GetElapsedMiliseconds() / 1000.0f);
	m_deltaTimer.Restart();
}
void Scene::CheckStopCondition()
{
	/*for (int i = 1; i < m_actors.size(); i++)
	{
		if (m_actors[i]->m_transform.GetPosition().y < 0.0f)
		{
			//m_sceneTimer.Stop();
			isPlaying = false;
		}
	}*/
}
void Scene::HandleEvent()
{
	// 윈도우 메시지 처리
	m_windowManager->m_window.HandleMessage();

	// 실시간 입력 이벤트 처리
	while (m_windowManager->m_mouse.IsEventBufferEmpty() == false)
	{
		MouseEvent mouseEvent = m_windowManager->m_mouse.ReadEvent();
		//if (windowManager.mouse.IsRightDown() == true)
		if (mouseEvent.GetType() == MouseEvent::Type::RAW_MOVE)
		{
			m_camera->m_transform.Rotate(-Vector3::Up() * (float)mouseEvent.GetPositionX() * m_windowManager->m_mouse.GetSensitivity());
			m_camera->m_transform.Rotate(-m_camera->m_transform.GetRight() * (float)mouseEvent.GetPositionY() * m_windowManager->m_mouse.GetSensitivity());
			m_camera->UpdateMatrix();
		}
	}
	while (m_windowManager->m_keyboard.IsCharBufferEmpty() == false)
	{
		unsigned char ch = m_windowManager->m_keyboard.ReadChar();
		/*if (ch == 'Q' || ch == 'q')
		{
			m_camera->m_transform.Translate(Vector3(-m_camera->m_transform.GetRight() * m_camera->GetSpeed() * m_deltaTime));
			m_camera->UpdateMatrix();
		}*/
	}
	while (m_windowManager->m_keyboard.IsKeyBufferEmpty() == false)
	{
		KeyboardEvent kbe = m_windowManager->m_keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	// 기존 입력 상태에 따른 이벤트 처리
	if (m_windowManager->m_keyboard.IsPressed('A'))
	{
		m_camera->m_transform.Translate(Vector3( - m_camera->m_transform.GetRight() * m_camera->GetSpeed() * m_deltaTime));
		m_camera->UpdateMatrix();
	}
	if (m_windowManager->m_keyboard.IsPressed('D'))
	{
		m_camera->m_transform.Translate(Vector3(m_camera->m_transform.GetRight() * m_camera->GetSpeed() * m_deltaTime));
		m_camera->UpdateMatrix();
	}
	if (m_windowManager->m_keyboard.IsPressed('E'))
	{
		for (int i = 0; i < m_actors.size(); i++)
		{
			m_actors[i]->m_rigidbody.AddForce(Vector3::Normalize(Vector3::Zero() - m_actors[i]->m_transform.GetPosition()) * -300.0f);
		}
	}
	if (m_windowManager->m_keyboard.IsPressed('F'))
	{
	}
	if (m_windowManager->m_keyboard.IsPressed('G'))
	{
		m_actors[1]->m_rigidbody.AddTorqueAt(Vector3(0, 0, -0.1f), m_actors[1]->m_transform.GetPosition() + Vector3(1, -1, 0));
	}
	if (m_windowManager->m_keyboard.IsPressed('Q'))
	{
		for (int i = 0; i < m_actors.size(); i++)
		{
			m_actors[i]->m_rigidbody.AddForce(Vector3::Normalize(Vector3::Zero() - m_actors[i]->m_transform.GetPosition()) * 300.0f);
		}
	}
	if (m_windowManager->m_keyboard.IsPressed('S'))
	{
		m_camera->m_transform.Translate(Vector3(m_camera->m_transform.GetForward() * m_camera->GetSpeed() * m_deltaTime));
		m_camera->UpdateMatrix();
	}
	if (m_windowManager->m_keyboard.IsPressed('W'))
	{
		m_camera->m_transform.Translate(Vector3( - m_camera->m_transform.GetForward() * m_camera->GetSpeed() * m_deltaTime));
		m_camera->UpdateMatrix();
	}
	if (m_windowManager->m_keyboard.IsPressed(VK_UP))
	{
		m_actors[1]->m_rigidbody.AddForce(Vector3::Normalize(m_actors[2]->m_transform.GetPosition() - m_actors[1]->m_transform.GetPosition()) * 3000.0f);
		m_actors[2]->m_rigidbody.AddForce(Vector3::Normalize(m_actors[1]->m_transform.GetPosition() - m_actors[2]->m_transform.GetPosition()) * 3000.0f);
	}
	if (m_windowManager->m_keyboard.IsPressed(VK_SPACE))
	{
		if (isPlaying == false)
		{
			isPlaying = true;
			//m_sceneTimer.Start();
			/*Model cubeModel;
			Transform transform;
			Rigidbody rigidbody;
			Collider collider;
			cubeModel.Initialize("Assets/Objects/Cube.obj", m_device.Get(), m_deviceContext.Get(), m_vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
			transform.Initialize(Vector3::Zero(), Vector3::Zero(), Vector3::One());
			rigidbody.Initialize(0.1f, 0.95f, 0.95f, Vector3::Zero(), Vector3::Zero(), Matrix4x4::CubeInertiaTensor(12.0f, transform.GetScale()).Inverse());
			collider.Initialize();
			Actor* bullet = new Actor();
			bullet->Initialize(cubeModel, transform, rigidbody, collider);

			bullet->m_transform.SetPosition(m_camera->m_transform.GetPosition());
			bullet->m_transform.SetScale(Vector3(1, 1, 1));
			bullet->m_rigidbody.SetInverseMass(0.1f);
			bullet->m_rigidbody.SetKinematic(false);
			bullet->m_rigidbody.SetInertiaTensorInverse(Matrix4x4::CubeInertiaTensor(12, bullet->m_transform.GetScale()));
			bullet->m_rigidbody.AddForceAt(Vector3(0, 0, -100000), bullet->m_transform.GetPosition() + Vector3(1, -1, 0));
			m_actors.push_back(bullet);*/
		}
	}
	if (m_windowManager->m_keyboard.IsPressed(VK_DOWN))
	{
		m_actors[1]->m_rigidbody.AddForce(Vector3::Normalize(m_actors[2]->m_transform.GetPosition() - m_actors[1]->m_transform.GetPosition()) * 10000.0f);
	}
	if (m_windowManager->m_keyboard.IsPressed(VK_CONTROL))
	{
		for (int i = 0; i < m_actors.size(); i++)
		{
			m_actors[i]->m_rigidbody.AddTorqueAt(Vector3(0, 0, -i *1), m_actors[i]->m_transform.GetPosition() + Vector3(1, 1, 0));
		}
	}
}
void Scene::UpdatePhysics()
{
	if (isPlaying == true)
		m_physicsManager->Update();
}
void Scene::UpdateScene()
{
	// Clear
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), m_camera->GetBackgroundColor());
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	// Input Assembler
	m_deviceContext->IASetInputLayout(m_vertexShader.GetInputLayout());
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// Vertex Shader
	m_deviceContext->VSSetShader(m_vertexShader.GetShader(), NULL, 0);
	
	// Rasterizer
	m_deviceContext->RSSetState(m_rasterizerState.Get());
	
	// Pixel Shader
	m_deviceContext->PSSetShader(m_pixelShader.GetShader(), NULL, 0);
	m_deviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	m_light->SetContantBuffer(m_psConstantBuffer, m_camera->m_transform.GetPosition());
	m_psConstantBuffer.ApplyChanges();
	m_deviceContext->PSSetConstantBuffers(0, 1, m_psConstantBuffer.GetAddressOf());
	
	// Output Merger
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
	m_deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);// 투명 쓸거면 첫번째 인자 "blendState.Get()"로

	// Draw Objects
	{
		for (int i = 0; i < m_actors.size(); i++)
		{
			m_deviceContext->VSSetConstantBuffers(0, 1, m_actors[i]->m_model.GetVSConstantBuffer()->GetAddressOf());
			// 여기부터 Model에 있던 코드
			for (int j = 0; j < m_actors[i]->m_model.m_meshes.size(); j++)
			{
				m_actors[i]->m_model.GetVSConstantBuffer()->data.wvpMatrix = m_camera->GetViewProjectionMatrix() * (m_actors[i]->m_transform.GetWorldMatrix() * m_actors[i]->m_model.m_meshes[j].m_worldMatrix);
				m_actors[i]->m_model.GetVSConstantBuffer()->data.worldMatrix = (m_actors[i]->m_transform.GetWorldMatrix() * m_actors[i]->m_model.m_meshes[j].m_worldMatrix);
				m_actors[i]->m_model.GetVSConstantBuffer()->ApplyChanges();
				// 여기부터 Mesh에 있던 코드
				{
					UINT offset = 0;
					for (int k = 0; k < m_actors[i]->m_model.m_meshes[j].m_textures.size(); k++)
					{
						if (m_actors[i]->m_model.m_meshes[j].m_textures[k].GetType() == aiTextureType::aiTextureType_DIFFUSE)
						{
							m_deviceContext->PSSetShaderResources(0, 1, m_actors[i]->m_model.m_meshes[j].m_textures[k].GetTextureResourceViewAddress());
							break;
						}
					}
					m_deviceContext->IASetVertexBuffers(0, 1, m_actors[i]->m_model.m_meshes[j].m_vertexbuffer.GetAddressOf(), m_actors[i]->m_model.m_meshes[j].m_vertexbuffer.GetAddressOfStride(), &offset);
					m_deviceContext->IASetIndexBuffer(m_actors[i]->m_model.m_meshes[j].m_indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
					m_deviceContext->DrawIndexed(m_actors[i]->m_model.m_meshes[j].m_indexbuffer.GetNumofIndex(), 0, 0);

				}
				m_actors[i]->m_model.m_meshes[j].UpdateWorldSpaceVertices(m_actors[i]->m_model.GetVSConstantBuffer()->data.worldMatrix);
			}
			m_actors[i]->Draw(m_camera->GetViewProjectionMatrix());
		}
		m_deviceContext->PSSetShader(m_pixelShaderNoLight.GetShader(), NULL, 0);
		m_light->Draw(m_camera->GetViewProjectionMatrix());
	}
	
	{// Draw Line
		
		m_basicEffect->SetMatrices(Matrix4x4::Identity().ToXMMATRIX(), m_camera->GetViewMatrix().ToXMMATRIX(), m_camera->GetProjectionMatrix().ToXMMATRIX());
		m_basicEffect->Apply(m_deviceContext.Get());
		DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(m_device.Get(), m_basicEffect.get(), m_primitiveBatchInputLayout.ReleaseAndGetAddressOf());
		m_deviceContext->IASetInputLayout(m_primitiveBatchInputLayout.Get());
		m_deviceContext->OMSetBlendState(m_blendState.Get(), NULL, 0xFFFFFFFF);// 투명 쓸거면 첫번째 인자 "blendState.Get()"로
		m_deviceContext->OMSetDepthStencilState(m_commonState->DepthNone(), 0);

		m_primitiveBatch->Begin();
		{
			DirectX::VertexPositionColor startVertex, endVertex;
			for (int i = 0; i < m_actors.size(); i++)
			{
				if (m_actors[i]->m_rigidbody.IsAwake() == false)
				{

					for (int meshCount = 0; meshCount < m_actors[i]->m_model.m_meshes.size(); meshCount++)
					{
						for (int indexCount = 0; indexCount < m_actors[i]->m_model.m_meshes[meshCount].m_indices.size(); indexCount += 3)
						{
							// 확인 결과 CCW 임
							Mesh mesh = m_actors[i]->m_model.m_meshes[meshCount];
							Vector3 vertices[3];
							vertices[0] = m_actors[i]->m_transform.GetWorldMatrix() * Vector3(mesh.m_vertices[mesh.m_indices[indexCount + 0]].position);
							vertices[1] = m_actors[i]->m_transform.GetWorldMatrix() * Vector3(mesh.m_vertices[mesh.m_indices[indexCount + 1]].position);
							vertices[2] = m_actors[i]->m_transform.GetWorldMatrix() * Vector3(mesh.m_vertices[mesh.m_indices[indexCount + 2]].position);
							startVertex = DirectX::VertexPositionColor(vertices[0].ToXMVECTOR(), DirectX::Colors::LightGreen);
							endVertex = DirectX::VertexPositionColor(vertices[1].ToXMVECTOR(), DirectX::Colors::LightGreen);
							m_primitiveBatch->DrawLine(startVertex, endVertex);
							startVertex = DirectX::VertexPositionColor(vertices[1].ToXMVECTOR(), DirectX::Colors::LightGreen);
							endVertex = DirectX::VertexPositionColor(vertices[2].ToXMVECTOR(), DirectX::Colors::LightGreen);
							m_primitiveBatch->DrawLine(startVertex, endVertex);
							startVertex = DirectX::VertexPositionColor(vertices[2].ToXMVECTOR(), DirectX::Colors::LightGreen);
							endVertex = DirectX::VertexPositionColor(vertices[0].ToXMVECTOR(), DirectX::Colors::LightGreen);
							m_primitiveBatch->DrawLine(startVertex, endVertex);
						}
					}
				}
			}
		}
		/*{
			// 마우스 클릭으로 물체 집을 수 있게 만들어보려다 일단 보류
			Vector3 mouseToWorldDir = (m_camera->GetViewProjectionMatrix().Inverse() * -Vector4(Vector3::Forward(), 0)).XYZ();
			Vector3 mouseToWorldPosition = m_camera->GetViewProjectionMatrix().Inverse() * Vector3((((float)m_windowManager->m_mouse.GetPositionX() / (float)m_windowManager->m_window.GetWidth()) * 2) - 1, (((float)-m_windowManager->m_mouse.GetPositionY() / (float)m_windowManager->m_window.GetHeight()) * 2) + 1, 0);
			DirectX::VertexPositionColor startVertex, endVertex;
			startVertex = DirectX::VertexPositionColor(Vector3::Zero().ToXMVECTOR(), DirectX::Colors::BlueViolet);
			endVertex = DirectX::VertexPositionColor((mouseToWorldPosition + mouseToWorldDir).ToXMVECTOR(), DirectX::Colors::BlueViolet);
			m_primitiveBatch->DrawLine(startVertex, endVertex);
		}*/
		/*{
			DirectX::VertexPositionColor startVertex, endVertex;
			for (int i = 0; i < m_lineForDebug[0].size(); i++)
			{
				startVertex = DirectX::VertexPositionColor(m_lineForDebug[0][i].ToXMVECTOR(), m_lineForDebug[2][i].ToXMVECTOR());
				endVertex = DirectX::VertexPositionColor(m_lineForDebug[1][i].ToXMVECTOR(), m_lineForDebug[2][i].ToXMVECTOR());
				m_primitiveBatch->DrawLine(startVertex, endVertex);
			}
			m_lineForDebug[0].clear();
			m_lineForDebug[1].clear();
			m_lineForDebug[2].clear();
		}*/
		/* {
			DirectX::VertexPositionColor startVertex, endVertex;
			for (int actorCount = 0; actorCount < actors.size(); actorCount++)
			{
				for (int meshCount = 0; meshCount < actors[actorCount]->model.meshes.size(); meshCount++)
				{
					for (int indexCount= 0; indexCount< actors[actorCount]->model.meshes[meshCount].indices.size() ; indexCount+=3)
					{
						// 확인 결과 CCW 임
						Mesh mesh = actors[actorCount]->model.meshes[meshCount];
						Vector3 edge1 = Vector3(mesh.vertices[mesh.indices[indexCount + 1]].pos) - Vector3(mesh.vertices[mesh.indices[indexCount]].pos);
						Vector3 edge2 = Vector3(mesh.vertices[mesh.indices[indexCount + 2]].pos) - Vector3(mesh.vertices[mesh.indices[indexCount]].pos);
						Vector3 centerPos = actors[actorCount]->transform.GetWorldMatrix() * ((Vector3(mesh.vertices[mesh.indices[indexCount]].pos) + Vector3(mesh.vertices[mesh.indices[indexCount + 1]].pos) + Vector3(mesh.vertices[mesh.indices[indexCount + 2]].pos)) /3);
						Vector3 normal = Vector3::Normalize(actors[actorCount]->transform.GetRotationMatrix() *Vector3::Cross(edge1, edge2));
						startVertex = DirectX::VertexPositionColor(centerPos.ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((centerPos + normal).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
					}
				}
			}
		}*/
		m_primitiveBatch->End();
		
		//{
		//	Vector4 lineColor;
		//	VertexPositionColor startVertex, endVertex;
		//	// 첫째 힘
		//	startVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0, 0, 1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	endVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0, 0, 2)).ToXMVECTOR(), DirectX::Colors::Red);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0, 0, 1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	endVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0.1, 0, 1.1f)).ToXMVECTOR(), DirectX::Colors::Red);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0, 0, 1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	endVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(-0.1, 0, 1.1f)).ToXMVECTOR(), DirectX::Colors::Red);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0.5, 0, 1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	endVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0.5, 0, 2)).ToXMVECTOR(), DirectX::Colors::Red);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0.5, 0, 1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	endVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0.6, 0, 1.1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0.5, 0, 1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	endVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0.4, 0, 1.1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(1, 0, 1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	endVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(1, 0, 2)).ToXMVECTOR(), DirectX::Colors::Red);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(1, 0, 1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	endVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(1.1, 0, 1.1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(1, 0, 1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	endVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(0.9, 0, 1.1)).ToXMVECTOR(), DirectX::Colors::Red);
		//	primitiveBatch->DrawLine(startVertex, endVertex);

		//	// 둘째 힘
		//	startVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0, -1, 1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	endVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0, -1, 2)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0, -1, 1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	endVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0.1, -1, 1.1f)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(0, -1, 1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	endVertex = VertexPositionColor((actors[1]->transform.GetPosition() + Vector3(-0.1, -1, 1.1f)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0, -1, 1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	endVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0, -1, 2)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0, -1, 1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	endVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0.1, -1, 1.1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(0, -1, 1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	endVertex = VertexPositionColor((actors[2]->transform.GetPosition() + Vector3(-0.1, -1, 1.1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(0, -1, 1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	endVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(0, -1, 2)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(0, -1, 1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	endVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(0.1, -1, 1.1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//	startVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(0, -1, 1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	endVertex = VertexPositionColor((actors[3]->transform.GetPosition() + Vector3(-0.1, -1, 1.1)).ToXMVECTOR(), DirectX::Colors::Blue);
		//	primitiveBatch->DrawLine(startVertex, endVertex);
		//}
		
		/*
		{
			Vector4 mainLineColor, subLineColor;
			DirectX::VertexPositionColor startVertex, endVertex;
			float cameraDistanceFromXZPlane = abs(camera->transform.GetPosition().y);
			float distanceLevel = 10;
			while (cameraDistanceFromXZPlane >= 10)
			{
				cameraDistanceFromXZPlane = cameraDistanceFromXZPlane / 10;
				distanceLevel *= 10;
			}
			mainLineColor = { 1.0f, 1.0f, 1.0f,  cameraDistanceFromXZPlane / 10.0f / 2.0f };
			subLineColor = { 1.0f, 1.0f, 1.0f, (10.0f - cameraDistanceFromXZPlane) / 10.0f / 2.0f};
			// x축 평행선
			int offsetCounter = -1;
			for (float offset = -50.0f; offset <= 50.0f; offset += 0.1f)
			{
				startVertex = DirectX::VertexPositionColor(DirectX::XMVectorSet(-100.0f * distanceLevel, 0.0f, offset * distanceLevel, 0.0f), subLineColor.ToXMVECTOR());
				endVertex = DirectX::VertexPositionColor(DirectX::XMVectorSet(100.0f * distanceLevel, 0.0f, offset * distanceLevel, 0.0f), subLineColor.ToXMVECTOR());
				primitiveBatch->DrawLine(startVertex, endVertex);
				startVertex = DirectX::VertexPositionColor(DirectX::XMVectorSet(offset * distanceLevel, 0.0f, -100.0f * distanceLevel, 0.0f), subLineColor.ToXMVECTOR());
				endVertex = DirectX::VertexPositionColor(DirectX::XMVectorSet(offset * distanceLevel, 0.0f, 100.0f * distanceLevel, 0.0f), subLineColor.ToXMVECTOR());
				primitiveBatch->DrawLine(startVertex, endVertex);
			}
			for (int offset = -50; offset <= 50; offset++)
			{
				startVertex = DirectX::VertexPositionColor(DirectX::XMVectorSet(-100.0f * distanceLevel, 0.0f, offset * distanceLevel, 0.0f), mainLineColor.ToXMVECTOR());
				endVertex = DirectX::VertexPositionColor(DirectX::XMVectorSet(100.0f * distanceLevel, 0.0f, offset * distanceLevel, 0.0f), mainLineColor.ToXMVECTOR());
				primitiveBatch->DrawLine(startVertex, endVertex);
				startVertex = DirectX::VertexPositionColor(DirectX::XMVectorSet(offset * distanceLevel, 0.0f, -100.0f * distanceLevel, 0.0f), mainLineColor.ToXMVECTOR());
				endVertex = DirectX::VertexPositionColor(DirectX::XMVectorSet(offset * distanceLevel, 0.0f, 100.0f * distanceLevel, 0.0f), mainLineColor.ToXMVECTOR());
				primitiveBatch->DrawLine(startVertex, endVertex);
			}
		}
		primitiveBatch->End();
		deviceContext->OMSetDepthStencilState(commonState->DepthNone(), 0);

		primitiveBatch->Begin();
		{
			DirectX::VertexPositionColor startVertex, endVertex;
			startVertex= DirectX::VertexPositionColor(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::Colors::Red);
			endVertex= DirectX::VertexPositionColor(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::Colors::Red);
			for (int i = 0; i < actors.size(); i++)
			{
				for (int j = i + 1; j < actors.size(); j++)
				{
					if (true)
					{
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[i]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);


						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, 0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(-0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
						startVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, 0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						endVertex = DirectX::VertexPositionColor((actors[j]->transform.GetWorldMatrix() * Vector3(0.5f, -0.5f, -0.5f)).ToXMVECTOR(), DirectX::Colors::Red);
						primitiveBatch->DrawLine(startVertex, endVertex);
					}
				}
			}
			primitiveBatch->End();
		}*/
	}
	
	UpdateUI();

	m_swapchain->Present(0, NULL);
}
void Scene::UpdateUI()
{
	//string firstActorVelocity = to_string(XMVectorGetX(actors[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetY(actors[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetZ(actors[0].rigidbody.velocity));
	//spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(firstActorVelocity).c_str(), DirectX::XMFLOAT2(0, 100), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	m_spriteBatch->Begin();
	//m_spriteFont->DrawString(m_spriteBatch.get(), StringHelper::StringToWString(to_string(m_sceneTimer.GetElapsedMiliseconds() / 1000.0f)).c_str(), DirectX::XMFLOAT2(5, 5), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
	//m_spriteFont->DrawString(m_spriteBatch.get(), StringHelper::StringToWString(m_fpsString).c_str(), DirectX::XMFLOAT2(5, 5), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
	/*Vector3 mouseToWorldPosition = m_camera->GetViewProjectionMatrix().Inverse() * Vector3((((float)m_windowManager->m_mouse.GetPositionX() / (float)m_windowManager->m_window.GetWidth()) * 2) - 1, (((float)-m_windowManager->m_mouse.GetPositionY() / (float)m_windowManager->m_window.GetHeight()) * 2) + 1, 0);
	m_spriteFont->DrawString(m_spriteBatch.get(), StringHelper::StringToWString(to_string(mouseToWorldPosition.x)).c_str(), DirectX::XMFLOAT2(5, 25), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
	m_spriteFont->DrawString(m_spriteBatch.get(), StringHelper::StringToWString(to_string(mouseToWorldPosition.y)).c_str(), DirectX::XMFLOAT2(5, 45), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
	m_spriteFont->DrawString(m_spriteBatch.get(), StringHelper::StringToWString(to_string(mouseToWorldPosition.z)).c_str(), DirectX::XMFLOAT2(5, 65), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));*/
	//for (int i = 1; i < m_actors.size(); i++)
	//{
	//	Vector3 actorVelocity= m_actors[i]->m_rigidbody.GetVelocity();// *180.0f / PI;
	//	string actorVelocityString = "Actor " + to_string(i) + " Velocity: " + to_string(actorVelocity.x) + ", " + to_string(actorVelocity.y) + ", " + to_string(actorVelocity.z);
	//	m_spriteFont->DrawString(m_spriteBatch.get(), StringHelper::StringToWString(actorVelocityString).c_str(), DirectX::XMFLOAT2(5, 25+i*45), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	//	Vector3 actorAngularVelocity = m_actors[i]->m_rigidbody.GetAngularVelocity();// *180.0f / PI;
	//	string actorAngularVelocityString = "Actor " + to_string(i)+ " AngularVelocity: " + to_string(actorAngularVelocity.x) + ", " + to_string(actorAngularVelocity.y) + ", " + to_string(actorAngularVelocity.z);
	//	m_spriteFont->DrawString(m_spriteBatch.get(), StringHelper::StringToWString(actorAngularVelocityString).c_str(), DirectX::XMFLOAT2(5, 45+i*45), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	//}
	/*
	string deltaTimeString = "DeltaTime: " + to_string(deltaTime);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(deltaTimeString).c_str(), DirectX::XMFLOAT2(5, 125), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));*/
	/*
	angularVelocity = actors[2]->rigidbody.GetAngularVelocity() * 180.0f / PI;
	string actor2AngularVelocity = "Center Object Angular Velocity: " + to_string((int)angularVelocity.x) + ", " + to_string((int)angularVelocity.y) + ", " + to_string((int)angularVelocity.z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(actor2AngularVelocity).c_str(), DirectX::XMFLOAT2(5, 60), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	angularVelocity = actors[3]->rigidbody.GetAngularVelocity() * 180.0f / PI;
	string actor3AngularVelocity = "Right Object Angular Velocity: " + to_string((int)angularVelocity.x) + ", " + to_string((int)angularVelocity.y) + ", " + to_string((int)angularVelocity.z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(actor3AngularVelocity).c_str(), DirectX::XMFLOAT2(5, 115), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	*/
	//string description= "Torque added position: 0, 0, 0 - 0.5, 0.5, 0.5 - 1, 1, 1 순";
	//spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(description).c_str(), DirectX::XMFLOAT2(5, 5), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	/*
	string actor2EulerRotation = "Actor[2] Euler Rotation: " + to_string(actors[2].transform.GetRotation().x) + ", " + to_string(actors[2].transform.GetRotation().y) + ", " + to_string(actors[2].transform.GetRotation().z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(actor2EulerRotation).c_str(), DirectX::XMFLOAT2(5, 60), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string actor3EulerRotation = "Actor[3] Euler Rotation: " + to_string(actors[3].transform.GetRotation().x) + ", " + to_string(actors[3].transform.GetRotation().y) + ", " + to_string(actors[3].transform.GetRotation().z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(actor3EulerRotation).c_str(), DirectX::XMFLOAT2(5, 90), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string groundObjectVelocity = "Ground Object Velocity: " + to_string(actors[0].rigidbody.velocity.x) + ", " + to_string(actors[0].rigidbody.velocity.y) + ", " + to_string(actors[0].rigidbody.velocity.z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(groundObjectVelocity).c_str(), DirectX::XMFLOAT2(5, 120), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string cameraPosition = "Camera Position: " + to_string(camera.transform.GetPosition().x) + ", " + to_string(camera.transform.GetPosition().y) + ", " + to_string(camera.transform.GetPosition().z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(cameraPosition).c_str(), DirectX::XMFLOAT2(5, 150), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string actor1QuaternionRotation = "Actor[1] Quaternion: " + to_string(actors[1].transform.GetRotationQuaternion().x) + ", " + to_string(actors[1].transform.GetRotationQuaternion().y) + ", " + to_string(actors[1].transform.GetRotationQuaternion().z) + ", " + to_string(actors[1].transform.GetRotationQuaternion().w);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(actor1QuaternionRotation).c_str(), DirectX::XMFLOAT2(5, 180), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	DirectX::XMVECTOR dxQuaternionRotation = DirectX::XMQuaternionRotationMatrix(actors[1].transform.GetRotationMatrix().ToXMMATRIX());
	string actor1DXQuaternionRotation = "Actor[1] Quaternion: " + to_string(XMVectorGetX(dxQuaternionRotation)) + ", " + to_string(XMVectorGetY(dxQuaternionRotation)) + ", " + to_string(XMVectorGetZ(dxQuaternionRotation)) + ", " + to_string(XMVectorGetW(dxQuaternionRotation));
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(actor1DXQuaternionRotation).c_str(), DirectX::XMFLOAT2(5, 210), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string contactNormal = "Contact Normal: " + to_string(XMVectorGetX(contact.contactNormal)) + ", " + to_string(XMVectorGetY(contact.contactNormal)) + ", " + to_string(XMVectorGetZ(contact.contactNormal));
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(contactNormal).c_str(), DirectX::XMFLOAT2(5, 90), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string separatingVelocity = "Separating Velocity: " + to_string(contact.separatingVelocity);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(separatingVelocity).c_str(), XMFLOAT2(5, 1200), DirectX::Colors::White, 0.0f, XMFLOAT2(0, 0), XMFLOAT2(1.0f, 1.0f));*/
	m_spriteBatch->End();

	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Light Controls");
	float lightStrength = m_light->GetStrength() ;
	ImGui::DragFloat("Ambient Light Strength", &lightStrength, 0.01f, 0.0f, 1.0f);
	m_light->SetStrength(lightStrength);
	/*ImGui::DragFloat3("Ambient Light Color", &psConstantBuffer.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	
	ImGui::NewLine();
	ImGui::DragFloat3("Dynamic Light Color", &light.dynamicColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("Dynamic Light Postion", &light.dynamicPosition.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Strength", &light.dynamicStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation A", &light.dynamicAttenA, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation B", &light.dynamicAttenB, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation C", &light.dynamicAttenC, 0.01f, 0.0f, 10.0f);*/
	//ImGui::Text("This is example text");
	//if (ImGui::Button("Click me"))
	//	counter++;
	//ImGui::SameLine();
	//string clickCount = "Click Count: " + to_string(counter);
	//ImGui::Text(clickCount.c_str());
	//ImGui::DragFloat3("Translation X/Y/Z", translationOffset, 0.1f, -5.0f, 5.0f);
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
}
Scene::~Scene()
{
	delete m_camera;
	delete m_light;
	for (int i = 0; i < m_actors.size(); i++)
	{
		delete m_actors[i];
	}
}