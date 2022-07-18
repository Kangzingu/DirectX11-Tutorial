#pragma once
#include "Utils/WIndow/WindowManager.h"
#include "Graphics/AdapterReader.h"
#include "Graphics/Shaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "Graphics/Camera3D.h"
#include "../Utils/Timer.h"
#include "Graphics/ImGui/imgui.h"
#include "Graphics/ImGui/imgui_impl_win32.h"
#include "Graphics/ImGui/imgui_impl_dx11.h"
#include "Graphics/RenderableGameObject.h"
#include "Graphics/Light.h"
#include "Graphics/Camera2D.h"
#include "Graphics/Sprite.h"

using namespace std;
using namespace Microsoft::WRL;

class Engine
{
public:
	void Initialize(HINSTANCE hInstance);
	void InitializeWindow(HINSTANCE hInstance);
	
	//Engine(HINSTANCE hInstance, string windowTitle, string windowClass, int windowWidth, int windowHeight);
	void InitializeDirectX();
	void InitializeShaders();
	void InitializeScene();
	void InitializeRenderer();
	void Run();
	bool IsWindowEnable();
	void UpdateDeltaTime();
	void ProcessDeviceInput();

	void Render();

	Camera3D camera3D;

	RenderableGameObject gameObject;
	RenderableGameObject gameObject2;

	vector<RenderableGameObject> gameObjects;
	void UpdatePhysics();

	Light light;
private:
	Timer timer;
	Timer sceneTimer;
	float deltaTime = 0;
	//Renderer renderer;// GFX == Renderer Effect
	WindowManager windowManager;
	int windowWidth = 0;
	int windowHeight = 0;

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;// 그리기를 전담하는 넘인듯
	ComPtr<IDXGISwapChain> swapchain;
	ComPtr<ID3D11RenderTargetView> renderTargetView;

	VertexShader vertexshader;
	PixelShader pixelshader;
	PixelShader pixelshader_nolight;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;

	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> depthStencilState;

	ComPtr<ID3D11RasterizerState> rasterizerState;
	ComPtr<ID3D11RasterizerState> rasterizerState_CULLFront;
	ComPtr<ID3D11BlendState> blendState;

	ComPtr<ID3D11SamplerState> samplerState;
	ComPtr<ID3D11ShaderResourceView> pinkTexture;
	ComPtr<ID3D11ShaderResourceView> grassTexture;
	ComPtr<ID3D11ShaderResourceView> pavementTexture;

	unique_ptr<DirectX::SpriteBatch> spriteBatch;
	unique_ptr<DirectX::SpriteFont> spriteFont;
};
