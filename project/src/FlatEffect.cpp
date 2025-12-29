#define NOMINMAX
#include <iostream>
#include "Texture.h"
#include "FlatEffect.h"

dae::FlatEffect::FlatEffect(ID3D11Device* pDevice) :
	BaseEffect(pDevice, L"Resources/Flat3D.fx")
{
	m_pDiffuseMapVariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable is not valid" << std::endl;
	}
}

dae::FlatEffect::~FlatEffect()
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->Release();
	if (m_pMatWorldVariable)
		m_pMatWorldVariable->Release();
}

void dae::FlatEffect::SetDiffuseMap(const Texture& pDiffuseTexture) const
{
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture.GetSRV());
	}
}