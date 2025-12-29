#pragma once

#include "BaseEffect.h"

namespace dae
{
	class FlatEffect final : public BaseEffect
	{
	public:
		FlatEffect(ID3D11Device* pDevice);
		~FlatEffect();
		FlatEffect(const FlatEffect&) = delete;
		FlatEffect(FlatEffect&&) noexcept = delete;
		FlatEffect& operator=(const FlatEffect&) = delete;
		FlatEffect& operator=(FlatEffect&&) noexcept = delete;
		void SetDiffuseMap(const Texture& pDiffuseTexture) const override;
	private:
		ID3DX11EffectMatrixVariable* m_pMatWorldVariable{};
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
	};
}