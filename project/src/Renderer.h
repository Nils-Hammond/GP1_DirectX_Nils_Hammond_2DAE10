#pragma once

#include <memory>

// Framework Headers
#include "Timer.h"
#include "Camera.h"
#include "Mesh.h"

namespace dae
{
	class HardwareRenderer;
	class SoftwareRenderer;
	class Texture;
	class Renderer final
	{
	public:
		enum class RendererType
		{
			Hardware,
			Software
		};
		Renderer(SDL_Window* pWindow, RendererType type = RendererType::Software);
		~Renderer();
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;
		void Update(Timer* pTimer);
		void Render();

		void SwapRenderer();
		void ToggleRotation();
		//void ToggleFire();
		void CycleSamplerState();
		void CycleShadingMode();
		void ToggleNormalMap();
		void ToggleDepthBufferVisualization();
		//void ToggleBoundingBoxVisualization();
	private:
		void InitializeScene();
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		std::unique_ptr<HardwareRenderer> m_pHardwareRenderer{};
		std::unique_ptr<SoftwareRenderer> m_pSoftwareRenderer{};
		RendererType m_RendererType{};

		bool m_IsRotating{ true };

		std::vector<Mesh*> m_Meshes{};

		Texture* m_pTexture{};
		Texture* m_pNormalMap{};
		Texture* m_pSpecularMap{};
		Texture* m_pGlossinessMap{};

		Texture* m_pFireTexture{};

		Camera m_Camera{};

	};
}