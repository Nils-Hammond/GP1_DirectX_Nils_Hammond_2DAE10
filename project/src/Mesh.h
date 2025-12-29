#pragma once

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

#include "ColorRGB.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Vector2.h"
#include <vector>
#include "BaseEffect.h"
#include "Matrix.h"
#include "Camera.h"

namespace dae
{
	struct Vertex
	{
		Vector3 position{};
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
	};

	struct Vertex_Out
	{
		Vector4 position{};
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector4 worldPosition{};
		Vector3 viewDirection{};
	};

	enum class ShaderType
	{
		Phong,
		Flat,
	};
	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};
	enum class SamplerStateType;
	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, ShaderType shaderType = ShaderType::Phong);
		~Mesh();
		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = delete;

		void Draw(ID3D11DeviceContext* pDeviceContext, const Camera& camera) const;
		void UpdateMatrix() const;
		void SetDiffuseMap(Texture* pTexture);
		void SetNormalMap(Texture* pNormalMap);
		void SetSpecularMap(Texture* pSpecularMap);
		void SetGlossinessMap(Texture* pGlossinessMap);
		void SetSamplerState(ID3D11SamplerState* pSamplerState) { m_pEffect->SetSamplerState(pSamplerState); }

		std::vector<Vertex> GetVertices() const { return m_Vertices; }
		std::vector<uint32_t> GetIndices() const { return m_Indices; }
		Texture* GetDiffuseTexture() const { return m_pDiffuseTexture; }
		Texture* GetNormalMap() const { return m_pNormalMap; }
		Texture* GetSpecularMap() const { return m_pSpecularMap; }
		Texture* GetGlossinessMap() const { return m_pGlossinessMap; }
		PrimitiveTopology GetPrimitiveTopology() const { return m_Topology; }
		ShaderType GetShaderType() const { return m_ShaderType; }

		Matrix m_WorldMatrix{};
	private:
		// DirectX
		ID3D11Buffer* m_pVertexBuffer{};
		ID3D11Buffer* m_pIndexBuffer{};
		uint32_t m_NumIndices{};

		BaseEffect* m_pEffect{};
		ID3D11InputLayout* m_pInputLayout{};

		// Software
		std::vector<Vertex> m_Vertices{};
		std::vector<uint32_t> m_Indices{};
		PrimitiveTopology m_Topology{ PrimitiveTopology::TriangleList };

		Texture* m_pDiffuseTexture{};
		Texture* m_pNormalMap{};
		Texture* m_pSpecularMap{};
		Texture* m_pGlossinessMap{};

		ShaderType m_ShaderType{};
	};
}