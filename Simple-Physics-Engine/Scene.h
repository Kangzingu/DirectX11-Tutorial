#pragma once
#include <CommonStates.h>
#include <d3d11.h>
#include <Effects.h>
#include <memory>
#include <PrimitiveBatch.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <SimpleMath.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <wrl.h>
#include "Graphics/Shaders/Shaders.h"
#include "Physics/Collision.h"
#include "Physics/PhysicsManager.h"
#include "Objects/Actor.h"
#include "Objects/Camera.h"
#include "Objects/Light.h"
#include "Utils/ImGui/imgui.h"
#include "Utils/ImGui/imgui_impl_win32.h"
#include "Utils/ImGui/imgui_impl_dx11.h"
#include "Utils/Timer.h"
#include "Utils/WIndow/WindowManager.h"
#include "DirectXHelpers.h"

using namespace std;
using namespace Microsoft::WRL;

class Scene
{
public:
	void Initialize(HINSTANCE hInstance);
	void InitializeWindow(HINSTANCE hInstance);
	void InitializePhysics();
	void InitializeDirectX();
	void InitializeShaders();
	void InitializeObjects();

	void Run();
	void HandleEvent();
	void UpdatePhysics();
	void UpdateTimer();
	void CheckStopCondition();
	void UpdateScene();
	void UpdateUI();
	bool IsRenderWindowExist();

	~Scene();
	void DestructScene();

private:
	Light* m_light;
	Camera* m_camera;
	vector<Actor*> m_actors;
	vector<Vector3> m_lineForDebug[3];

	Timer m_fpsTimer;
	Timer m_deltaTimer;
	//Timer m_sceneTimer;
	string m_fpsString = "FPS: 0";
	int m_fps = 0;
	float m_deltaTime = 0;
	bool isPlaying = false;

	WindowManager* m_windowManager;
	PhysicsManager* m_physicsManager;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_deviceContext;// 그리기를 전담하는 넘인듯
	ComPtr<IDXGISwapChain> m_swapchain;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	VertexShader m_vertexShader;
	PixelShader m_pixelShader;
	PixelShader m_pixelShaderNoLight;
	ConstantBuffer<VSConstantBuffer> m_vsConstantBuffer;
	ConstantBuffer<PSConstantBuffer> m_psConstantBuffer;

	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	ComPtr<ID3D11RasterizerState> m_rasterizerState;
	ComPtr<ID3D11RasterizerState> m_rasterizerState_CULLFront;
	ComPtr<ID3D11BlendState> m_blendState;

	ComPtr<ID3D11SamplerState> m_samplerState;

	unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	unique_ptr<DirectX::SpriteFont> m_spriteFont;

	unique_ptr<DirectX::CommonStates> m_commonState;
	unique_ptr<DirectX::BasicEffect> m_basicEffect;
	unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
	ComPtr<ID3D11InputLayout> m_primitiveBatchInputLayout;
};