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

//class ParticleContact
//{
//	// 121pg 읽어야함
//public:
//	Actor* actors[2];
//	float restitution;// 반발계수(충돌 깊이의 역할)
//	Vector3 contactNormal;// actor[0]이 actor[1]로 다가가는
//	float penetration;// 충돌 깊이(언젠간 구해야함..ㅎ)
//	float separatingVelocity;
//
//public:
//	void Resolve(float duration)
//	{
//		// 최종적인 접촉 해결 과정은 다음과 같을거임
//		// 1. 각 충돌 별 separating velocity를 구하고 가장 심하게 충돌된 걸 고름
//		// 2. 만약 가장 심한게 0보다 크면(충돌이 아니면) 그냥 끝
//		// 3. 0보다 작으면 그에 대한 해결을 함(Impulse로 속도 바꾸고.. 겹친 부분 떼주고..)
//		// 4. 1단계로 돌아감
//		ResolveVelocity(duration);
//		ResolveInterpenetration(duration);
//	}
//	float CalculateSeparatingVelocity() const
//	{
//		Vector3 relativeVelocity = actors[0]->rigidbody.velocity;
//		if (actors[1] != nullptr)
//			relativeVelocity -= actors[1]->rigidbody.velocity;
//		return Vector3::Dot(relativeVelocity, contactNormal);
//	}
//
//private:
//	void ResolveVelocity(float duration)
//	{
//		// 두 물체의 상대속력(접촉속력)을 구함
//		separatingVelocity = CalculateSeparatingVelocity();
//
//		if (separatingVelocity > 0)// 접촉이 아니라 멀어지고 있는거라면
//			return;
//
//		// 총 충격량을 구함(충돌 후 운동량 - 충돌 전 운동량)
//		float newSepVelocity = -separatingVelocity * restitution;
//
//		Vector3 accCausedVelocity = actors[0]->rigidbody.accumulatedForce / actors[0]->rigidbody.mass;
//		if (actors[1] != nullptr) accCausedVelocity -= actors[1]->rigidbody.accumulatedForce / actors[1]->rigidbody.mass;
//		float accCausedSepVelocity = Vector3::Dot(accCausedVelocity, contactNormal) * duration;
//		if (accCausedSepVelocity < 0)
//		{
//			newSepVelocity += restitution * accCausedSepVelocity;
//			if (newSepVelocity < 0) newSepVelocity = 0;
//		}
//
//		float deltaVelocity = newSepVelocity - separatingVelocity;
//
//		float totalInverseMass = 1.0f / actors[0]->rigidbody.mass;
//		if (actors[1] != nullptr) totalInverseMass += 1.0f / actors[1]->rigidbody.mass;
//
//		if (totalInverseMass <= 0) return;
//
//		// 일케하면 (상대 물체 질량 / 총 질량의 합)따라 영향을 받음(질량이 크면 충격에 따른 속도 변화가 작아져야 하므로)
//		float impulse = deltaVelocity / totalInverseMass;
//
//		Vector3 impulsePerIMess = contactNormal * impulse;
//
//		actors[0]->rigidbody.velocity = actors[0]->rigidbody.velocity + impulsePerIMess / actors[0]->rigidbody.mass;
//
//		if (actors[1] != nullptr)
//		{
//			actors[1]->rigidbody.velocity = actors[1]->rigidbody.velocity + impulsePerIMess / -actors[1]->rigidbody.mass;
//		}
//	}
//	void ResolveInterpenetration(float duration)
//	{
//		// 113pg 참고
//		if (penetration <= 0) return;
//		float totalInverseMass = 1.0f / actors[0]->rigidbody.mass;
//		if (actors[1] != nullptr) totalInverseMass += 1.0f / actors[1]->rigidbody.mass;
//		if (totalInverseMass <= 0) return;
//		Vector3 movePerIMass = contactNormal * (penetration / totalInverseMass);
//		actors[0]->transform.position += movePerIMass / actors[0]->rigidbody.mass;
//		if (actors[1] != nullptr) actors[1]->transform.position+=movePerIMass / -actors[1]->rigidbody.mass;
//	}
//};
//class ParticleContactResolver
//{
//protected:
//	unsigned int iterations;
//	unsigned int iterationsUsed;
//
//public:
//	ParticleContactResolver(unsigned int iterations)
//	{
//
//	}
//	void SetIterations(unsigned int iterations)
//	{
//
//	}
//	void ResolveContacts(ParticleContact* contactArray, unsigned int numContacts, float duration)
//	{
//		iterationsUsed = 0;
//		while (iterationsUsed < iterations)
//		{
//			float max = 0;
//			unsigned int maxIndex = numContacts;
//			for (unsigned int i = 0; i < numContacts; i++)
//			{
//				float sepVel = contactArray[i].CalculateSeparatingVelocity();
//				if (sepVel < max)
//				{
//					max = sepVel;
//					maxIndex = i;
//				}
//			}
//			contactArray[maxIndex].Resolve(duration);
//			iterationsUsed++;
//		}
//	}
//};
//class ParticleLink
//{// 충돌 해결을 역으로 생각해서 물체가 멀어지면 가까워지게 하는 식으로, 오히려 연결된 것 처럼 행동하게 해보는거임
//public:
//	Actor* actor[2];
//protected:
//	float currentLength() const;
//public:
//	virtual unsigned int fillContact(ParticleContact* contact, unsigned int limit) const = 0;
//	
//};
//class ParticleCable : public ParticleLink
//{
//public:
//	float maxLength;
//	float restitution;
//public:
//	float CurrentLength() const
//	{
//		return Vector3::Magnitude(actor[0]->transform.position - actor[1]->transform.position);
//	}
//	unsigned int fillContact(ParticleContact* contact, unsigned int limit) const
//	{
//		float length = CurrentLength();
//		if (length < maxLength) 
//			return 0;
//
//		contact->actors[0] = actor[0];
//		contact->actors[1] = actor[1];
//
//		contact->contactNormal = Vector3::Normalize(actor[1]->transform.position - actor[0]->transform.position);
//		
//		contact->penetration = length - maxLength;
//		contact->restitution = restitution;
//
//		return 1;
//	}
//};
//ParticleContact contact;

int spacePressedCount = 0;

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
		UpdateTimer();
		HandleEvent();
		UpdatePhysics();
		UpdateScene();
	}
}
void Engine::InitializeWindow(HINSTANCE hInstance)
{
	this->windowManager.Initialize(hInstance, "Simple Physics Engine", "Default", 1920, 1080);
}
void Engine::InitializeDirectX()
{
	// 스왑 체인
	vector<AdapterData> graphicsAdapter = AdapterReader::GetAdapters();
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	ComPtr<ID3D11Texture2D> swapChainBuffer;
	swapChainDescription.OutputWindow = this->windowManager.window.GetHWND();
	swapChainDescription.BufferDesc.Width = this->windowManager.window.GetWidth();
	swapChainDescription.BufferDesc.Height = this->windowManager.window.GetHeight();
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
	ERROR_IF_FAILED(D3D11CreateDeviceAndSwapChain(graphicsAdapter[0].pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, NULL, 0, D3D11_SDK_VERSION, &swapChainDescription, swapchain.GetAddressOf(), device.GetAddressOf(), NULL, deviceContext.GetAddressOf()), "디바이스와 스왑체인 생성에 실패했습니다");
	ERROR_IF_FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(swapChainBuffer.GetAddressOf())), "스왑체인의 GetBuffer 함수 호출에 실패했습니다");
	ERROR_IF_FAILED(device->CreateRenderTargetView(swapChainBuffer.Get(), NULL, renderTargetView.GetAddressOf()), "렌더타겟 뷰 생성에 실패했습니다");

	// 뷰포트
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowManager.window.GetWidth()), static_cast<float>(this->windowManager.window.GetHeight()));
	deviceContext->RSSetViewports(1, &viewport);

	// 라스터라이저
	CD3D11_RASTERIZER_DESC rasterizerDescription(D3D11_DEFAULT);
	rasterizerDescription.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;// D3D11_CULL_MODE::D3D11_CULL_NONE;//D3D11_CULL_MODE::D3D11_CULL_FRONT; // 참고로 OpenGL은 ccw, DirectX는 cw임
	ERROR_IF_FAILED(device->CreateRasterizerState(&rasterizerDescription, rasterizerState.GetAddressOf()), "라스터라이저 상태 생성에 실패했습니다");

	// 깊이 테스트
	CD3D11_TEXTURE2D_DESC depthStencilTextureDescription(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowManager.window.GetWidth(), this->windowManager.window.GetHeight());
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
	/* TODO: 이거 두줄 Light에 psConstantBuffer 넘겨서 없애기 */
	psConstantBuffer.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	psConstantBuffer.data.ambientLightStrength = 0.5f;

	// 액터
	Actor actor;
	actor.Initialize("Assets/Objects/Cube.obj", device.Get(), deviceContext.Get(), vsConstantBuffer, aiColor3D(0.0f, 0.0f, 1.0f));
	actors.push_back(actor);
	actor.Initialize("Assets/Objects/Cube.obj", device.Get(), deviceContext.Get(), vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
	actors.push_back(actor);
	actors.push_back(actor);
	actors.push_back(actor);
	actors[0].rigidbody.isKinematic=true;
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i].transform.SetPosition(Vector3(i * 0.0f, i * 10.0f, 0.0f));
	}
	actors[1].rigidbody.momentOfInertia = Matrix4x4::MomentOfInertiaCube(actors[1].rigidbody.mass, Vector3::One());
	actors[2].rigidbody.momentOfInertia = Matrix4x4::MomentOfInertiaSphere(actors[2].rigidbody.mass, 1);
	actors[3].rigidbody.momentOfInertia = Matrix4x4::MomentOfInertiaEmptySphere(actors[3].rigidbody.mass, 1);

	actors[1].transform.Rotate(Vector3(0, 0, 0));
	actors[2].transform.Rotate(Vector3(0, PI*2.0f, 0));
	actors[3].transform.Rotate(Vector3(0, PI*3.0f, 0));

	// 조명
	light.Initialize(device.Get(), deviceContext.Get(), vsConstantBuffer, aiColor3D(1.0f, 1.0f, 1.0f));
	light.transform.SetPosition(Vector3(3.0f, 5.0f, 0.0f));

	// 카메라
	camera.Initialize(45.0f, static_cast<float>(this->windowManager.window.GetWidth()) / static_cast<float>(this->windowManager.window.GetHeight()), 0.1f, 3000.0f);
	camera.transform.SetPosition(Vector3(0.0f, 5.0f, 10.0f));
	// 타이머
	sceneTimer.Start();
	fpsTimer.Start();
}
void Engine::HandleEvent()
{
	// 윈도우 메시지 처리
	windowManager.window.HandleMessage();

	// 마우스 이벤트
	while (!windowManager.mouse.IsEventBufferEmpty())
	{
		MouseEvent mouseEvent = windowManager.mouse.ReadEvent();
		//if (windowManager.mouse.IsRightDown() == true)
		if (mouseEvent.GetType() == MouseEvent::Type::RAW_MOVE)
		{
			this->camera.transform.Rotate(Vector3::Up() * (float)mouseEvent.GetPosX() * -0.001f);
			this->camera.transform.Rotate(camera.transform.GetRight() * (float)mouseEvent.GetPosY() * -0.001f);
			camera.UpdateMatrix();
		}
	}

	// 키보드 이벤트
	float cameraSpeed = -10.0f;
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
		this->camera.transform.Translate(Vector3(this->camera.transform.GetForward() * cameraSpeed * deltaTime));
		camera.UpdateMatrix();
	}
	if (windowManager.keyboard.KeyIsPressed('A'))
	{
		this->camera.transform.Translate(Vector3(this->camera.transform.GetRight() * cameraSpeed * deltaTime));
		camera.UpdateMatrix();
	}
	if (windowManager.keyboard.KeyIsPressed('S'))
	{
		this->camera.transform.Translate(Vector3( - this->camera.transform.GetForward() * cameraSpeed * deltaTime));
		camera.UpdateMatrix();
	}
	if (windowManager.keyboard.KeyIsPressed('D'))
	{
		this->camera.transform.Translate(Vector3( - this->camera.transform.GetRight() * cameraSpeed * deltaTime));
		camera.UpdateMatrix();
	}
	if (windowManager.keyboard.KeyIsPressed(VK_SPACE))
	{
		spacePressedCount++;
		actors[1].rigidbody.AddTorque(Vector3(0, 0, 1.0f), actors[1].transform.GetPosition() + Vector3(-1, 1, 1), actors[1].transform);
		// 지금 잘 돌아가는 이유는 내가 로컬좌표계로 주고있기 때문임
		// 결국 월드좌표계로 힘과 지점을 알려주게 되면 내부에선
		// 1. 힘(벡터)에 회전 역행렬을 곱함
		// 2. 지점(점)에 월드 역행렬을 곱함
		// 3. 이걸 크로스 해서 나온 결과를 쿼터니언 행렬로서 구하고 이걸 곱해줘야함
		// 지금은 크로스 해서 나온 결과(회전축과 회전할 크기 벡터)를 오일러값으로 바로 바꿔서 일케된듯..? 함
		// 이걸 잘 누적시키려면 결국 쿼터니언의 연산을 제대로 구현해야함

		camera.UpdateMatrix();
	}
	if (windowManager.keyboard.KeyIsPressed(VK_CONTROL))
	{
		actors[1].rigidbody.AddTorque(Vector3(-1.0f, 0, 0), actors[1].transform.GetPosition() + Vector3(0, 0,1), actors[1].transform);
	}
}
void Engine::UpdatePhysics()
{
	for (int i = 0; i < actors.size(); i++)
	{
		//forceGenerator.GenerateGravity(actors[i].rigidbody);
		actors[i].rigidbody.Update(actors[i].transform, deltaTime);
	}/*
	actors[1].transform.rotation += Vector3(1, 1, 1) * deltaTime;
	actors[2].transform.rotation += Vector3(-1, 1, 1) * deltaTime;
	actors[3].transform.rotation += Vector3(1, 1, -1) * deltaTime;*/
	//actors[1].rigidbody.AddTorque(Vector3(0, 0, 1), actors[1].transform.GetPosition() + Vector3(1, 1, 1), actors[1].transform);
	//actors[2].rigidbody.AddTorque(Vector3(0, 0, 1), actors[2].transform.GetPosition() + Vector3(1, 1, 1), actors[2].transform);
	//actors[3].rigidbody.AddTorque(Vector3(0, 0, 1), actors[3].transform.GetPosition() + Vector3(1, 1, 1), actors[3].transform);
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
	psConstantBuffer.data.dynamicLightPosition = light.transform.GetPosition().ToXMFLOAT3();
	psConstantBuffer.data.dynamicLightAttenuation_a = light.attenuation_a;
	psConstantBuffer.data.dynamicLightAttenuation_b = light.attenuation_b;
	psConstantBuffer.data.dynamicLightAttenuation_c = light.attenuation_c;
	psConstantBuffer.ApplyChanges();
	deviceContext->PSSetConstantBuffers(0, 1, psConstantBuffer.GetAddressOf());
	deviceContext->RSSetState(rasterizerState.Get());
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);// 투명 쓸거면 첫번째 인자 "blendState.Get()"로

	{// 오브젝트 그리기
		Matrix4x4 viewProjectionMatrix;
		for (int i = 0; i < actors.size(); i++)
		{
			viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
			actors[i].Draw(viewProjectionMatrix);
		};	
		deviceContext->PSSetShader(pixelShaderNoLight.GetShader(), NULL, 0);
		light.Draw(viewProjectionMatrix);
	}

	{// 선 그리기
		//std::unique_ptr<DirectX::CommonStates> m_states;
		//m_states = std::make_unique<CommonStates>(device.Get());
		unique_ptr<CommonStates> commonState;
		unique_ptr<BasicEffect> basicEffect;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> primitiveBatchInputLayout;
		commonState = make_unique<CommonStates>(device.Get());
		basicEffect = make_unique<BasicEffect>(device.Get());
		basicEffect->SetVertexColorEnabled(true);
		basicEffect->SetMatrices(SimpleMath::Matrix::Identity, camera.viewMatrix.ToXMMATRIX(), camera.projectionMatrix.ToXMMATRIX());
		basicEffect->Apply(deviceContext.Get());
		CreateInputLayoutFromEffect<VertexPositionColor>(device.Get(), basicEffect.get(), primitiveBatchInputLayout.ReleaseAndGetAddressOf());
		deviceContext->IASetInputLayout(primitiveBatchInputLayout.Get());
		deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);// 투명 쓸거면 첫번째 인자 "blendState.Get()"로

		unique_ptr<PrimitiveBatch<VertexPositionColor>> primitiveBatch;
		primitiveBatch = make_unique<PrimitiveBatch<VertexPositionColor>>(deviceContext.Get());
		primitiveBatch->Begin();

		Vector4 lineColor;
		Vector3 waterSurface = actors[0].transform.GetPosition();
		VertexPositionColor v1(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::Colors::White);
		VertexPositionColor v2(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::Colors::White);
		for (int i = 1; i < actors.size(); i++)
		{
			waterSurface.x = actors[i].transform.GetPosition().x;
			waterSurface.z = actors[i].transform.GetPosition().z;
			v1 = VertexPositionColor(waterSurface.ToXMVECTOR(), DirectX::Colors::White);
			float lineLength = Vector3::Magnitude(waterSurface - actors[i].transform.GetPosition());
			lineColor = { 1.0f / lineLength, 1.0f / lineLength, 1.0f, 1.0f };
			v2 = VertexPositionColor(actors[i].transform.GetPosition().ToXMVECTOR(), lineColor.ToXMVECTOR());
			primitiveBatch->DrawLine(v1, v2);
		}
		{
			Vector4 mainLineColor, subLineColor;
			VertexPositionColor startVertex, endVertex;
			float cameraDistanceFromXZPlane = abs(camera.transform.GetPosition().y);
			float distanceLevel = 10;
			while (cameraDistanceFromXZPlane >= 10)
			{
				cameraDistanceFromXZPlane = cameraDistanceFromXZPlane / 10;
				distanceLevel *= 10;
			}
			mainLineColor = { 1.0f, 1.0f, 1.0f,  cameraDistanceFromXZPlane / 10.0f / 3.0f };
			subLineColor = { 1.0f, 1.0f, 1.0f, (10.0f - cameraDistanceFromXZPlane) / 10.0f / 3.0f };
			// x축 평행선
			for (int offset = -50; offset <= 50; offset++)
			{
				startVertex = VertexPositionColor(XMVectorSet(-100.0f * distanceLevel, 0.0f, offset * distanceLevel, 0.0f), mainLineColor.ToXMVECTOR());
				endVertex = VertexPositionColor(XMVectorSet(100.0f * distanceLevel, 0.0f, offset * distanceLevel, 0.0f), mainLineColor.ToXMVECTOR());
				primitiveBatch->DrawLine(startVertex, endVertex);
			}
			for (float offset = -50.0f; offset <= 50.0f; offset += 0.1f)
			{
				startVertex = VertexPositionColor(XMVectorSet(-100.0f * distanceLevel, 0.0f, offset * distanceLevel, 0.0f), subLineColor.ToXMVECTOR());
				endVertex = VertexPositionColor(XMVectorSet(100.0f * distanceLevel, 0.0f, offset * distanceLevel, 0.0f), subLineColor.ToXMVECTOR());
				primitiveBatch->DrawLine(startVertex, endVertex);
			}

			// z축 평행선
			for (int offset = -50; offset <= 50; offset++)
			{
				startVertex = VertexPositionColor(XMVectorSet(offset * distanceLevel, 0.0f, -100.0f * distanceLevel, 0.0f), mainLineColor.ToXMVECTOR());
				endVertex = VertexPositionColor(XMVectorSet(offset * distanceLevel, 0.0f, 100.0f * distanceLevel, 0.0f), mainLineColor.ToXMVECTOR());
				primitiveBatch->DrawLine(startVertex, endVertex);
			}
			for (float offset = -50.0f; offset <= 50.0f; offset += 0.1f)
			{
				startVertex = VertexPositionColor(XMVectorSet(offset * distanceLevel, 0.0f, -100.0f * distanceLevel, 0.0f), subLineColor.ToXMVECTOR());
				endVertex = VertexPositionColor(XMVectorSet(offset * distanceLevel, 0.0f, 100.0f * distanceLevel, 0.0f), subLineColor.ToXMVECTOR());
				primitiveBatch->DrawLine(startVertex, endVertex);
			}
			primitiveBatch->End();
		}
	}

	UpdateUI();

	swapchain->Present(0, NULL);
}
void Engine::UpdateUI()
{
	//string firstActorVelocity = to_string(XMVectorGetX(actors[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetY(actors[0].rigidbody.velocity)) + ", " + to_string(XMVectorGetZ(actors[0].rigidbody.velocity));
	//spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(firstActorVelocity).c_str(), DirectX::XMFLOAT2(0, 100), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fps).c_str(), XMFLOAT2(5, 5), DirectX::Colors::White, 0.0f, XMFLOAT2(0, 0), XMFLOAT2(1.0f, 1.0f));
	string movingObjectVelocity1 = "Actor[1] Euler Rotation: " + to_string(actors[1].transform.GetRotation().x) + ", " + to_string(actors[1].transform.GetRotation().y) + ", " + to_string(actors[1].transform.GetRotation().z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(movingObjectVelocity1).c_str(), DirectX::XMFLOAT2(5, 30), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string movingObjectVelocity2 = "Actor[2] Euler Rotation: " + to_string(actors[2].transform.GetRotation().x) + ", " + to_string(actors[2].transform.GetRotation().y) + ", " + to_string(actors[2].transform.GetRotation().z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(movingObjectVelocity2).c_str(), DirectX::XMFLOAT2(5, 60), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string movingObjectVelocity3 = "Actor[3] Euler Rotation: " + to_string(actors[3].transform.GetRotation().x) + ", " + to_string(actors[3].transform.GetRotation().y) + ", " + to_string(actors[3].transform.GetRotation().z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(movingObjectVelocity3).c_str(), DirectX::XMFLOAT2(5, 90), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string groundObjectVelocity = "Ground Object Velocity: " + to_string(actors[0].rigidbody.velocity.x) + ", " + to_string(actors[0].rigidbody.velocity.y) + ", " + to_string(actors[0].rigidbody.velocity.z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(groundObjectVelocity).c_str(), DirectX::XMFLOAT2(5, 120), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string cameraPosition = "Camera Position: " + to_string(camera.transform.GetPosition().x) + ", " + to_string(camera.transform.GetPosition().y) + ", " + to_string(camera.transform.GetPosition().z);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(cameraPosition).c_str(), DirectX::XMFLOAT2(5, 150), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string spacePressedCountString = "Space Pressed Count"+ to_string(spacePressedCount);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(spacePressedCountString).c_str(), XMFLOAT2(5, 180), DirectX::Colors::White, 0.0f, XMFLOAT2(0, 0), XMFLOAT2(1.0f, 1.0f));
	/*string contactNormal = "Contact Normal: " + to_string(XMVectorGetX(contact.contactNormal)) + ", " + to_string(XMVectorGetY(contact.contactNormal)) + ", " + to_string(XMVectorGetZ(contact.contactNormal));
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(contactNormal).c_str(), DirectX::XMFLOAT2(5, 90), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	string separatingVelocity = "Separating Velocity: " + to_string(contact.separatingVelocity);
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(separatingVelocity).c_str(), XMFLOAT2(5, 1200), DirectX::Colors::White, 0.0f, XMFLOAT2(0, 0), XMFLOAT2(1.0f, 1.0f));*/
	spriteBatch->End();

	
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
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
bool Engine::IsRenderWindowExist()
{
	return windowManager.window.IsEnable();
}

