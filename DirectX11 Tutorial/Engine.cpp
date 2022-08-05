#include "Engine.h"
// 1. 조건식에 !연산자는 사용 x
// 2. SAL / noexcept 사용 금지(OUT, INOUT은 필요에 따라 사용 가능)
// 3. 클래스 선언
/*	class C
	{
	public:
		//변수

	public:
		//함수

	protected:
		//변수

	protected:
		//함수

	private:
		//변수

	private:
		//함수
	};
*/
// 4. 주석은 최대한 없도록
// 5. 클래스 초기화 등 함수 이름
// - createActor() / destroyActor()
// - initializeActor()
// 6. 약자는 모두 대문자로 ex) ui -> UI
// 7. 들여쓰기 최대한 안하게
/*
- 나쁜예
bool process()
{
	if (hasBall == true)
	{
		for (int i = 0; i < length; i++)
		{
			// do
		}
	}
}

 - 좋은예
bool process()
{
	if (hasBall == false)
		return false;

	for (int i = 0; i < length; i++)
	{
		// do
	}
*/
// 8. 벡터 순회 방법
/*
- 나쁜예
for (int i = 0; i < numItems.size(); i++)
{
	// do
}

 - 좋은예
 for (Item& item : items)
{
	// do
}
*/
void Engine::Initialize(HINSTANCE hInstance)
{
	InitializeWindow(hInstance);
	InitializePhysics();
	InitializeDirectX();
	InitializeShaders();
	InitializeScene();
}
void Engine::Run()
{
	while (IsRenderWindowExist())
	{
		UpdateTimer();
		HandleEvent();
		UpdatePhysics();
		UpdateScene();
	}
}
void Engine::InitializeWindow(HINSTANCE hInstance)
{
	windowManager = new WindowManager(hInstance, "Simple Physics Engine", "Default", 1600, 900);
}
void Engine::InitializePhysics()
{
	physicsManager = new PhysicsManager(&actors, deltaTime, lineForDebug);
}
void Engine::InitializeDirectX()
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
	swapChainDescription.OutputWindow = windowManager->window.GetHWND();
	swapChainDescription.BufferDesc.Width = windowManager->window.GetWidth();
	swapChainDescription.BufferDesc.Height = windowManager->window.GetHeight();
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
	ERROR_IF_FAILED(D3D11CreateDeviceAndSwapChain(adapters[0], D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, NULL, 0, D3D11_SDK_VERSION, &swapChainDescription, swapchain.GetAddressOf(), device.GetAddressOf(), NULL, deviceContext.GetAddressOf()), "디바이스와 스왑체인 생성에 실패했습니다");
	ERROR_IF_FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(swapChainBuffer.GetAddressOf())), "스왑체인의 GetBuffer 함수 호출에 실패했습니다");
	ERROR_IF_FAILED(device->CreateRenderTargetView(swapChainBuffer.Get(), NULL, renderTargetView.GetAddressOf()), "렌더타겟 뷰 생성에 실패했습니다");

	// 뷰포트
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowManager->window.GetWidth()), static_cast<float>(windowManager->window.GetHeight()));
	deviceContext->RSSetViewports(1, &viewport);

	// 라스터라이저
	CD3D11_RASTERIZER_DESC rasterizerDescription(D3D11_DEFAULT);
	rasterizerDescription.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;// D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // 참고로 OpenGL은 ccw, DirectX는 cw임
	ERROR_IF_FAILED(device->CreateRasterizerState(&rasterizerDescription, rasterizerState.GetAddressOf()), "라스터라이저 상태 생성에 실패했습니다");

	// 깊이 테스트
	CD3D11_TEXTURE2D_DESC depthStencilTextureDescription(DXGI_FORMAT_D24_UNORM_S8_UINT, windowManager->window.GetWidth(), windowManager->window.GetHeight());
	depthStencilTextureDescription.MipLevels = 1;
	depthStencilTextureDescription.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	ERROR_IF_FAILED(device->CreateTexture2D(&depthStencilTextureDescription, NULL, depthStencilBuffer.GetAddressOf()), "Depth Stencil 텍스쳐 생성에 실패했습니다");
	ERROR_IF_FAILED(device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf()), "Depth Stencil 뷰 생성에 실패했습니다");
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	CD3D11_DEPTH_STENCIL_DESC depthStencilDescription(D3D11_DEFAULT);
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;// 일케하면 깊이가 같은 경우에는 걍 넘어가는듯, LESS_EQUAL은 같은 경우 늦게 들어온넘을 그리고
	ERROR_IF_FAILED(device->CreateDepthStencilState(&depthStencilDescription, depthStencilState.GetAddressOf()), "Depth Stencil 상태 생성에 실패했습니다");

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
	spriteBatch = make_unique<DirectX::SpriteBatch>(deviceContext.Get());
	spriteFont = make_unique<DirectX::SpriteFont>(device.Get(), L"Assets/Fonts/NanumSquare_ac_36.spritefont");

	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(windowManager->window.GetHWND());
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	ImGui::StyleColorsDark();

	// 2D
	commonState = make_unique<DirectX::CommonStates>(device.Get());
	basicEffect = make_unique<DirectX::BasicEffect>(device.Get());
	basicEffect->SetVertexColorEnabled(true);
	primitiveBatch = make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(deviceContext.Get());
}
void Engine::InitializeShaders()
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
	vertexShader.Initialize(device, shaderPath + L"vertexshader.cso", inputElementDescription, numofElements);
	pixelShader.Initialize(device, shaderPath + L"pixelshader.cso");
	pixelShaderNoLight.Initialize(device, shaderPath + L"pixelshader_nolight.cso");
	vsConstantBuffer.Initialize(device.Get(), deviceContext.Get());
	psConstantBuffer.Initialize(device.Get(), deviceContext.Get());
}
void Engine::InitializeScene()
{
	// 컴포넌트 초기화
	Actor* actor;
	Model model;
	Model lightModel;
	Transform transform;
	Rigidbody rigidbody;
	Collider collider;

	// 액터
	model.Initialize("Assets/Objects/Cube.obj", device.Get(), deviceContext.Get(), vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
	transform.Initialize(Vector3::Zero(), Vector3::Zero(), Vector3::One());
	rigidbody.Initialize(1.0f, 1.0f, 1.0f, Vector3::Zero(), Vector3::Zero(), Matrix4x4::CubeInertiaTensor(1.0f, transform.GetScale()));
	collider.Initialize();
	actor = new Actor();
	actor->Initialize(model, transform, rigidbody, collider);
	actor->transform.SetPosition(Vector3(0, 0, 0));
	//actor->transform.SetRotation(Vector3(1, 0, 0));
	actor->transform.SetScale(Vector3(10, 1, 10));
	actor->rigidbody.SetInverseMass(0);
	actor->rigidbody.SetInertiaTensor(Matrix4x4::CubeInertiaTensor(0, actor->transform.GetScale()));
	actors.push_back(actor);
	for (int i = 1; i < 5; i++)
	{
		actor = new Actor();
		actor->Initialize(model, transform, rigidbody, collider);
		actor->transform.SetPosition(Vector3(0, i * 5, 0));
		actors.push_back(actor);
	}
	//actors[1].transform.Rotate(Vector4(0, 0, 0.5f, sqrt(3.0f) / 2.0f));
	//actors[2].transform.Rotate(Vector3(0, 1, 0), 30.0f);
	//actors[3].transform.Rotate(Vector3(0, 0, 0));

	// 조명
	lightModel.Initialize("Assets/Objects/Light.obj", device.Get(), deviceContext.Get(), vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
	transform.SetPosition(Vector3(3.0f, 10.0f, 0.0f));
	rigidbody.SetEnabled(false);
	collider.SetEnabled(false);
	light = new Light();
	light->Initialize(lightModel, transform, rigidbody, collider);
	light->SetAmbientLight(psConstantBuffer, Vector3::One(), 0.5f);
	light->SetDynamicLight(psConstantBuffer, Vector3::One(), 7.0f, 1.0f, 0.1, 0.1f);

	// 카메라
	model.isEnabled = false;
	transform.SetPosition(Vector3(0.0f, 8.0f, 8.0f));
	transform.Rotate(Vector3(0, 0, 0));
	rigidbody.SetEnabled(false);
	collider.SetEnabled(false);
	camera = new Camera();
	camera->Initialize(model, transform, rigidbody, collider);
	camera->SetProjectionMatrix(45.0f, static_cast<float>(windowManager->window.GetWidth()) / static_cast<float>(windowManager->window.GetHeight()), 0.1f, 3000.0f);
	
	// 타이머
	sceneTimer.Start();
	fpsTimer.Start();

	// 배경 색
	backgroundColor[0] = 0;
	backgroundColor[1] = 0;
	backgroundColor[2] = 0;
	backgroundColor[3] = 1;

	actors[0]->transform.Rotate(Vector3(0, General::DegreeToRadian(45.0f), 0));
	/*actors[1]->transform.SetRotation(Vector3(0, 45, 0));
	actors[2]->transform.SetRotation(Vector3(0, 0, 45));
	actors[3]->transform.SetRotation(Vector3(1, 0, 0));
	actors[4]->transform.SetRotation(Vector3(1, 0, 0));*/
}
bool Engine::IsRenderWindowExist()
{
	return windowManager->window.IsEnable();
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
void Engine::HandleEvent()
{
	// 윈도우 메시지 처리
	windowManager->window.HandleMessage();

	// 마우스 이벤트
	while (!windowManager->mouse.IsEventBufferEmpty())
	{
		MouseEvent mouseEvent = windowManager->mouse.ReadEvent();
		//if (windowManager.mouse.IsRightDown() == true)
		if (mouseEvent.GetType() == MouseEvent::Type::RAW_MOVE)
		{
			camera->transform.Rotate(Vector3::Up() * (float)mouseEvent.GetPosX() * -0.0005f);
			camera->transform.Rotate(camera->transform.GetRight() * (float)mouseEvent.GetPosY() * -0.0005f );
			Vector3 cameraRotation = camera->transform.GetRotation();
			camera->UpdateMatrix();
		}
	}

	// 키보드 이벤트
	float cameraSpeed = 10.0f;
	while (!windowManager->keyboard.IsCharBufferEmpty())
	{
		unsigned char ch = windowManager->keyboard.ReadChar();
	}
	while (!windowManager->keyboard.IsKeyBufferEmpty())
	{
		KeyboardEvent kbe = windowManager->keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	if (windowManager->keyboard.KeyIsPressed('W'))
	{
		camera->transform.Translate(Vector3( - camera->transform.GetForward() * cameraSpeed * deltaTime));
		camera->UpdateMatrix();
	}
	if (windowManager->keyboard.KeyIsPressed('A'))
	{
		camera->transform.Translate(Vector3( - camera->transform.GetRight() * cameraSpeed * deltaTime));
		camera->UpdateMatrix();
	}
	if (windowManager->keyboard.KeyIsPressed('S'))
	{
		camera->transform.Translate(Vector3(camera->transform.GetForward() * cameraSpeed * deltaTime));
		camera->UpdateMatrix();
	}
	if (windowManager->keyboard.KeyIsPressed('D'))
	{
		camera->transform.Translate(Vector3(camera->transform.GetRight() * cameraSpeed * deltaTime));
		camera->UpdateMatrix();
	}
	if (windowManager->keyboard.KeyIsPressed('Q'))
	{
		actors[1]->rigidbody.AddForceAt(Vector3(1, 0, 0), actors[1]->transform.GetPosition() + Vector3(0, 0, 0));
		actors[3]->rigidbody.AddForceAt(Vector3(-1, 0, 0), actors[1]->transform.GetPosition() + Vector3(0, 0, 0));
	}
	if (windowManager->keyboard.KeyIsPressed('G'))
	{
		actors[1]->rigidbody.AddTorqueAt(Vector3(0, 0, -0.1f), actors[1]->transform.GetPosition() + Vector3(1, -1, 0));
	}
	if (windowManager->keyboard.KeyIsPressed(VK_SPACE))
	{
		actors[1]->transform.Translate((actors[2]->transform.GetPosition() - actors[1]->transform.GetPosition()) * 0.0005f);
		actors[2]->transform.Translate((actors[1]->transform.GetPosition() - actors[2]->transform.GetPosition()) * 0.0005f);

	}
	if (windowManager->keyboard.KeyIsPressed(VK_CONTROL))
	{
		for (int i = 0; i < actors.size(); i++)
		{
			actors[i]->rigidbody.AddTorqueAt(Vector3(0, 0, -0.1f * i), actors[i]->transform.GetPosition() + Vector3(1, 1, 0));
		}
	}
}
void Engine::UpdatePhysics()
{
	physicsManager->Update();
}
void Engine::UpdateScene()
{
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), backgroundColor);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	deviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	light->SetContantBuffer(psConstantBuffer);
	psConstantBuffer.ApplyChanges();
	deviceContext->PSSetConstantBuffers(0, 1, psConstantBuffer.GetAddressOf());
	deviceContext->RSSetState(rasterizerState.Get());
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);// 투명 쓸거면 첫번째 인자 "blendState.Get()"로

	Matrix4x4 viewProjectionMatrix = camera->projectionMatrix * camera->viewMatrix;
	{// 오브젝트 그리기
		for (int i = 0; i < actors.size(); i++)
		{
			actors[i]->Draw(viewProjectionMatrix);
		}
		deviceContext->PSSetShader(pixelShaderNoLight.GetShader(), NULL, 0);
		light->Draw(viewProjectionMatrix);
	}
	
	{// 선 그리기
		
		basicEffect->SetMatrices(Matrix4x4::Identity().ToXMMATRIX(), camera->viewMatrix.ToXMMATRIX(), camera->projectionMatrix.ToXMMATRIX());
		basicEffect->Apply(deviceContext.Get());
		DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(device.Get(), basicEffect.get(), primitiveBatchInputLayout.ReleaseAndGetAddressOf());
		deviceContext->IASetInputLayout(primitiveBatchInputLayout.Get());
		deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);// 투명 쓸거면 첫번째 인자 "blendState.Get()"로
		deviceContext->OMSetDepthStencilState(commonState->DepthNone(), 0);

		primitiveBatch->Begin();
		{
			DirectX::VertexPositionColor startVertex, endVertex;
			for (int i = 0; i < lineForDebug[0].size(); i++)
			{
				startVertex = DirectX::VertexPositionColor(lineForDebug[0][i].ToXMVECTOR(), DirectX::Colors::Red);
				endVertex = DirectX::VertexPositionColor(lineForDebug[1][i].ToXMVECTOR(), DirectX::Colors::Red);
				primitiveBatch->DrawLine(startVertex, endVertex);
			}
			lineForDebug[0].clear();
			lineForDebug[1].clear();
		}
		primitiveBatch->End();
		
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

	swapchain->Present(0, NULL);
}
void Engine::UpdateUI()
{
	//string firstActorVelocity = to_string(XMVectorGetX(actors[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetY(actors[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetZ(actors[0].rigidbody.velocity));
	//spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(firstActorVelocity).c_str(), DirectX::XMFLOAT2(0, 100), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fps).c_str(), DirectX::XMFLOAT2(5, 5), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
	Vector3 actor1Position = camera->transform.GetPosition();// *180.0f / PI;
	string actor1PositionString = "Camera Position: " + to_string((int)actor1Position.x) + ", " + to_string((int)actor1Position.y) + ", " + to_string((int)actor1Position.z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(actor1PositionString).c_str(), DirectX::XMFLOAT2(5, 45), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	Vector3 actor1Velocity= actors[0]->transform.GetRotation();// *180.0f / PI;
	string actor1VelocityString = "Camera Rotation: " + to_string((int)actor1Velocity.x) + ", " + to_string((int)actor1Velocity.y) + ", " + to_string((int)actor1Velocity.z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(actor1VelocityString).c_str(), DirectX::XMFLOAT2(5, 85), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
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
	spriteBatch->End();

	/*
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Light Controls");
	ImGui::DragFloat3("Ambient Light Color", &psConstantBuffer.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &psConstantBuffer.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	
	ImGui::NewLine();
	ImGui::DragFloat3("Dynamic Light Color", &light.dynamicColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("Dynamic Light Postion", &light.dynamicPosition.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Strength", &light.dynamicStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation A", &light.dynamicAttenA, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation B", &light.dynamicAttenB, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation C", &light.dynamicAttenC, 0.01f, 0.0f, 10.0f);
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
	*/
}
void Engine::DestructScene()
{
	delete camera;
	delete light;
	for (int i = 0; i < actors.size(); i++)
	{
		delete actors[i];
	}
}