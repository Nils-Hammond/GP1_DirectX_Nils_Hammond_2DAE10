#define NOMINMAX
#include "BaseEffect.h"
#include "Texture.h"
#include "Matrix.h"
#include <iostream>
#include <sstream>

static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect{ nullptr };

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob)
		{
			const char* pErrors = static_cast<const char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

dae::BaseEffect::BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	m_pEffect = LoadEffect(pDevice, assetFile);
	if (!m_pEffect)
	{
		return;
	}
	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechnique->IsValid())
	{
		std::wcerr << L"m_pTechnique is not valid" << std::endl;
	}
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcerr << L"m_pMatWorldViewProjVariable is not valid" << std::endl;
	}
	m_pSamplerVariable = m_pEffect->GetVariableByName("gSamplerState")->AsSampler();
	if (!m_pSamplerVariable->IsValid())
	{
		std::wcerr << L"m_pSamplerVariable is not valid" << std::endl;
	}
}

dae::BaseEffect::~BaseEffect()
{
	if (m_pSamplerVariable)
		m_pSamplerVariable->Release();
	if (m_pMatWorldViewProjVariable)
		m_pMatWorldViewProjVariable->Release();
	if (m_pTechnique)
		m_pTechnique->Release();
	if (m_pEffect)
		m_pEffect->Release();
}

void dae::BaseEffect::SetWorldViewProjMatrix(const Matrix& matrix) const
{
	if (m_pMatWorldViewProjVariable)
	{
		float matArr[4][4];
		matrix.AsColMajArray(matArr);
		m_pMatWorldViewProjVariable->SetMatrix(&matArr[0][0]);
	}
}

void dae::BaseEffect::SetSamplerState(ID3D11SamplerState* pSamplerState) const
{
	if (m_pSamplerVariable)
	{
		m_pSamplerVariable->SetSampler(0, pSamplerState);
	}
}