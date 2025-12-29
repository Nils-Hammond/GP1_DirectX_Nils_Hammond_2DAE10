#pragma once

#include "BaseEffect.h"

#include <string>

namespace dae
{
	class Effect final : public BaseEffect
	{
	public:
		Effect(ID3D11Device* pDevice);
		~Effect();
		Effect(const Effect&) = delete;
		Effect(Effect&&) noexcept = delete;
		Effect& operator=(const Effect&) = delete;
		Effect& operator=(Effect&&) noexcept = delete;

		void SetWorldMatrix(const Matrix& matrix) const override;
		void SetDiffuseMap(const Texture& pDiffuseTexture) const override;
		void SetNormalMap(const Texture& pNormalTexture) const override;
		void SetSpecularMap(const Texture& pSpecularTexture) const override;
		void SetGlossinessMap(const Texture& pGlossinessTexture) const override;
		void SetCameraOrigin(const Vector3& cameraOrigin) const override;
	private:
		ID3DX11EffectMatrixVariable* m_pMatWorldVariable{};
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};
		ID3DX11EffectVectorVariable* m_pCameraOriginVariable{};
	};
}