#pragma once
#include "Renderer.h"

namespace dae
{
	class Texture;
	struct ColorRGB;
	struct Vertex;
	struct Vertex_Out;
	class SoftwareRenderer
	{
	public:
		SoftwareRenderer(SDL_Window* pWindow);
		~SoftwareRenderer() = default;
		SoftwareRenderer(const SoftwareRenderer&) = delete;
		SoftwareRenderer(SoftwareRenderer&&) noexcept = delete;
		SoftwareRenderer& operator=(const SoftwareRenderer&) = delete;
		SoftwareRenderer& operator=(SoftwareRenderer&&) noexcept = delete;

		void Render(std::vector<Mesh*> meshes, const Camera& camera);

		void ToggleDepthBufferVisualization();
		void ToggleNormalMap();
		void CycleShadingMode();
	private:
		bool CullTriangle(const Vertex_Out& v0, const Vertex_Out& v1, const Vertex_Out& v2) const;
		float DepthRemap(float value, float min, float max) const;
		void Render_Meshes(std::vector<Mesh*> meshes, const Camera& camera);
		ColorRGB PixelShading(const Vertex_Out& interpolatedVertex, const Mesh& mesh) const;
		void VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out, const Matrix& worldMatrix, const Camera& camera) const;
		bool IndicesIntermediaryFunction(const Mesh& mesh, uint32_t& v0Index, uint32_t& v1Index, uint32_t& v2Index, size_t& i) const;
		bool SaveBufferToImage() const;

		SDL_Window* m_pWindow{};
		int m_Width{};
		int m_Height{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		enum class ShadingMode
		{
			ObservedArea,
			Diffuse,
			Specular,
			Combined
		};

		ShadingMode m_ShadingMode{ ShadingMode::Combined };

		bool m_DepthBufferVisualizationEnabled{ false };
		bool m_IsNormalMapEnabled{ true };
	};
}