#pragma once
// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

#include <string>

namespace dae
{
	class Texture;
	struct Matrix;
	struct Vector3;
	class BaseEffect
	{
	public:
		BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~BaseEffect();
		BaseEffect(const BaseEffect&) = delete;
		BaseEffect(BaseEffect&&) noexcept = delete;
		BaseEffect& operator=(const BaseEffect&) = delete;
		BaseEffect& operator=(BaseEffect&&) noexcept = delete;

		//ID3DX11Effect* GetEffect() const { return m_pEffect; }
		ID3DX11EffectTechnique* GetTechnique() const { return m_pTechnique; }
		void SetWorldViewProjMatrix(const Matrix& matrix) const;
		void SetSamplerState(ID3D11SamplerState* pSamplerState) const;
		virtual void SetWorldMatrix(const Matrix& matrix) const {};
		virtual void SetDiffuseMap(const Texture& pDiffuseTexture) const {};
		virtual void SetNormalMap(const Texture& pNormalTexture) const {};
		virtual void SetSpecularMap(const Texture& pSpecularTexture) const {};
		virtual void SetGlossinessMap(const Texture& pGlossinessTexture) const {};
		virtual void SetCameraOrigin(const Vector3& cameraOrigin) const {};
	protected:
		ID3DX11Effect* GetEffect() const { return m_pEffect; }
		ID3DX11EffectMatrixVariable* GetWorldViewProjVariable() const { return m_pMatWorldViewProjVariable; }
		ID3DX11EffectSamplerVariable* GetSamplerVariable() const { return m_pSamplerVariable; }
	private:
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
		ID3DX11EffectSamplerVariable* m_pSamplerVariable{};
	};
}