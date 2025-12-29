#define NOMINMAX
#include "Effect.h"
#include "Matrix.h"
#include "Texture.h"
#include <iostream>
#include <sstream>

static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

dae::Effect::Effect(ID3D11Device* pDevice) :
	BaseEffect(pDevice, L"Resources/PosCol3D.fx")
{
	m_pMatWorldVariable = GetEffect()->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldVariable->IsValid())
	{
		std::wcout << L"m_pMatWorldVariable is not valid" << std::endl;
	}
	m_pDiffuseMapVariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable is not valid" << std::endl;
	}
	m_pNormalMapVariable = GetEffect()->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
	{
		std::wcout << L"m_pNormalMapVariable is not valid" << std::endl;
	}
	m_pSpecularMapVariable = GetEffect()->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
	{
		std::wcout << L"m_pSpecularMapVariable is not valid" << std::endl;
	}
	m_pGlossinessMapVariable = GetEffect()->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
	{
		std::wcout << L"m_pGlossinessMapVariable is not valid" << std::endl;
	}
	m_pCameraOriginVariable = GetEffect()->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_pCameraOriginVariable->IsValid())
	{
		std::wcout << L"m_pCameraOriginVariable is not valid" << std::endl;
	}
}

dae::Effect::~Effect()
{
	if (m_pNormalMapVariable)
		m_pNormalMapVariable->Release();
	if (m_pSpecularMapVariable)
		m_pSpecularMapVariable->Release();
	if (m_pGlossinessMapVariable)
		m_pGlossinessMapVariable->Release();
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->Release();
	if (m_pMatWorldVariable)
		m_pMatWorldVariable->Release();
	if (m_pCameraOriginVariable)
		m_pCameraOriginVariable->Release();
}

void dae::Effect::SetWorldMatrix(const Matrix& matrix) const
{
	if (m_pMatWorldVariable)
	{
		float matArray[4][4];
		matrix.AsColMajArray(matArray);
		m_pMatWorldVariable->SetMatrix(&matArray[0][0]);
	}
}

void dae::Effect::SetDiffuseMap(const Texture& pDiffuseTexture) const
{
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture.GetSRV());
	}
}

void dae::Effect::SetNormalMap(const Texture& pNormalTexture) const
{
	if (m_pNormalMapVariable)
	{
		m_pNormalMapVariable->SetResource(pNormalTexture.GetSRV());
	}
}

void dae::Effect::SetSpecularMap(const Texture& pSpecularTexture) const
{
	if (m_pSpecularMapVariable)
	{
		m_pSpecularMapVariable->SetResource(pSpecularTexture.GetSRV());
	}
}

void dae::Effect::SetGlossinessMap(const Texture& pGlossinessTexture) const
{
	if (m_pGlossinessMapVariable)
	{
		m_pGlossinessMapVariable->SetResource(pGlossinessTexture.GetSRV());
	}
}

void dae::Effect::SetCameraOrigin(const Vector3& cameraOrigin) const
{
	if (m_pCameraOriginVariable)
	{
		m_pCameraOriginVariable->SetFloatVector(&cameraOrigin.x);
	}
}

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