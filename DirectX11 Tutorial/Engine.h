#pragma once
#include "DirectXHelpers.h"
#include "Graphics/AdapterReader.h"
#include "Graphics/Camera.h"
#include "Graphics/Collision.h"
#include "Graphics/ForceGenerator.h"
#include "Graphics/ImGui/imgui.h"
#include "Graphics/ImGui/imgui_impl_win32.h"
#include "Graphics/ImGui/imgui_impl_dx11.h"
#include "Graphics/Light.h"
#include "Graphics/Actor.h"
#include "Graphics/Shaders.h"
#include "Utils/Timer.h"
#include "Utils/WIndow/WindowManager.h"
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

using namespace std;
using namespace Microsoft::WRL;

class Engine
{
public:
	void Initialize(HINSTANCE hInstance);
	void InitializeWindow(HINSTANCE hInstance);
	void InitializeDirectX();
	void InitializeShaders();
	void InitializeScene();

	void Run();
	void HandleEvent();
	void UpdatePhysics();
	void UpdateTimer();
	void UpdateScene();
	void UpdateUI();
	bool IsRenderWindowExist();

	void DestructScene();
private:
	float backgroundColor[4];

	Light* light;
	Camera* camera;
	vector<Actor*> actors;

	Timer fpsTimer;
	Timer sceneTimer;
	string fps = "FPS: 0";
	int fpsCount = 0;
	float deltaTime = 0;

	WindowManager windowManager;
	ForceGenerator forceGenerator;

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;// 그리기를 전담하는 넘인듯
	ComPtr<IDXGISwapChain> swapchain;
	ComPtr<ID3D11RenderTargetView> renderTargetView;

	VertexShader vertexShader;
	PixelShader pixelShader;
	PixelShader pixelShaderNoLight;
	ConstantBuffer<VSConstantBuffer> vsConstantBuffer;
	ConstantBuffer<PSConstantBuffer> psConstantBuffer;

	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> depthStencilState;

	ComPtr<ID3D11RasterizerState> rasterizerState;
	ComPtr<ID3D11RasterizerState> rasterizerState_CULLFront;
	ComPtr<ID3D11BlendState> blendState;

	ComPtr<ID3D11SamplerState> samplerState;

	unique_ptr<SpriteBatch> spriteBatch;
	unique_ptr<SpriteFont> spriteFont;

	unique_ptr<CommonStates> commonState;
	unique_ptr<BasicEffect> basicEffect;
	unique_ptr<PrimitiveBatch<VertexPositionColor>> primitiveBatch;
	ComPtr<ID3D11InputLayout> primitiveBatchInputLayout;
};