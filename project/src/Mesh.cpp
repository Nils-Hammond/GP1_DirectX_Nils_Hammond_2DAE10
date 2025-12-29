#define NOMINMAX
#include "Mesh.h"
#include "Matrix.h"
#include "Renderer.h"
#include <cassert>
#include "FlatEffect.h"
#include "Effect.h"

dae::Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, ShaderType shaderType)
	: m_Vertices(vertices), m_Indices(indices), m_ShaderType(shaderType)
{
	// Instantiate Effect
	if (shaderType == ShaderType::Flat)
	{
		m_pEffect = new FlatEffect(pDevice);
	}
	else if (shaderType == ShaderType::Phong)
	{
		m_pEffect = new Effect(pDevice);
	}

	if (!m_pEffect)
		return;
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechnique();
	if (!pTechnique || !pTechnique->IsValid())
		return;
	
	// Create Vertex Layout
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
		assert(false);

	// Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = static_cast<uint32_t>(sizeof(Vertex) * vertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = vertices.data();

	result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	// Create Index Buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;

}

dae::Mesh::~Mesh()
{
	if (m_pVertexBuffer)	m_pVertexBuffer->Release();
	if (m_pIndexBuffer)		m_pIndexBuffer->Release();
	if (m_pInputLayout)		m_pInputLayout->Release();
	if (m_pEffect)			delete m_pEffect;
}

void dae::Mesh::Draw(ID3D11DeviceContext* pDeviceContext, const Camera& camera) const
{
	if (!pDeviceContext || !m_pEffect || !m_pInputLayout || !m_pVertexBuffer || !m_pIndexBuffer)
		return;

	const Matrix worldViewProj = m_WorldMatrix * camera.viewMatrix * camera.projectionMatrix;
	m_pEffect->SetWorldViewProjMatrix(worldViewProj);

	m_pEffect->SetCameraOrigin(camera.origin);

	//1. Set primitive topology
	if (m_Topology == PrimitiveTopology::TriangleList)
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	else if (m_Topology == PrimitiveTopology::TriangleStrip)
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//2. Set Input Layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//3. Set Vertex Buffer
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//4. Set Index Buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//5. Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	//std::string techniqueName{};
	//switch (sampleState)
	//{
	//case SamplerStateType::Point:
	//	techniqueName = "PointSamplingTechnique";
	//	break;
	//case SamplerStateType::Linear:
	//	techniqueName = "LinearSamplingTechnique";
	//	break;
	//case SamplerStateType::Anisotropic:
	//	techniqueName = "AnisotropicSamplingTechnique";
	//	break;
	//}

	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	//ID3DX11EffectTechnique* pTechnique = m_pEffect->GetEffect()->GetTechniqueByName(techniqueName.c_str());
	//pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		//pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void dae::Mesh::UpdateMatrix() const
{
	if (!m_pEffect)
		return;
	m_pEffect->SetWorldMatrix(m_WorldMatrix);
}

void dae::Mesh::SetDiffuseMap(Texture* texture)
{
	m_pDiffuseTexture = texture;
	if (!m_pEffect)
		return;
	m_pEffect->SetDiffuseMap(*texture);
}

void dae::Mesh::SetNormalMap(Texture* pNormalMap)
{
	m_pNormalMap = pNormalMap;
	if (!m_pEffect)
		return;
	m_pEffect->SetNormalMap(*pNormalMap);
}

void dae::Mesh::SetSpecularMap(Texture* pSpecularMap)
{
	m_pSpecularMap = pSpecularMap;
	if (!m_pEffect)
		return;
	m_pEffect->SetSpecularMap(*pSpecularMap);
}

void dae::Mesh::SetGlossinessMap(Texture* pGlossinessMap)
{
	m_pGlossinessMap = pGlossinessMap;
	if (!m_pEffect)
		return;
	m_pEffect->SetGlossinessMap(*pGlossinessMap);
}