#pragma once

// SDL Headers
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_surface.h"
#include "SDL_image.h"

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// Framework Headers
#include "Timer.h"
#include "Camera.h"

namespace dae
{
	enum class SamplerStateType
	{
		Point,
		Linear,
		Anisotropic
	};
	class Mesh;
	class Texture;
	class HardwareRenderer final
	{
	public:
		HardwareRenderer(SDL_Window* pWindow);
		~HardwareRenderer();

		HardwareRenderer(const HardwareRenderer&) = delete;
		HardwareRenderer(HardwareRenderer&&) noexcept = delete;
		HardwareRenderer& operator=(const HardwareRenderer&) = delete;
		HardwareRenderer& operator=(HardwareRenderer&&) noexcept = delete;

		void Render(const std::vector<Mesh*>& meshes, const Camera& camera) const;
		void CycleSamplerState(const std::vector<Mesh*>& meshes);
		ID3D11Device* GetDevice() const { return m_pDevice; }
	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		SamplerStateType m_SamplerStateType{ SamplerStateType::Point };

		//DIRECTX
		HRESULT InitializeDirectX();

		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		IDXGISwapChain* m_pSwapChain{};
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};
		ID3D11Texture2D* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};
		//...
	};
}
