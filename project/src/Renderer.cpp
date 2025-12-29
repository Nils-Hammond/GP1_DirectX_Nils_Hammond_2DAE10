#include "Renderer.h"
#include "HardwareRenderer.h"
#include "SoftwareRenderer.h"
#include "Texture.h"
#include "Mesh.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow, RendererType type)
	: m_pWindow(pWindow)
	, m_RendererType(type)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	m_pHardwareRenderer = std::make_unique<HardwareRenderer>(pWindow);
	m_pSoftwareRenderer = std::make_unique<SoftwareRenderer>(pWindow);

	InitializeScene();
}

Renderer::~Renderer()
{
	delete m_pTexture;
	delete m_pNormalMap;
	delete m_pSpecularMap;
	delete m_pGlossinessMap;
	delete m_pFireTexture;
	for (auto& mesh : m_Meshes)
	{
		delete mesh;
	}
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
	if (m_IsRotating)
	{
		for (auto& mesh : m_Meshes)
		{
			mesh->m_WorldMatrix = Matrix::CreateRotationY(pTimer->GetElapsed() * PI / 4) * mesh->m_WorldMatrix;
			mesh->UpdateMatrix();
		}
	}
}

void Renderer::Render()
{
	if (m_RendererType == RendererType::Hardware)
	{
		m_pHardwareRenderer->Render(m_Meshes, m_Camera);
	}
	else
	{
		m_pSoftwareRenderer->Render(m_Meshes, m_Camera);
	}
}

void dae::Renderer::SwapRenderer()
{
	if (m_RendererType == RendererType::Hardware)
	{
		m_RendererType = RendererType::Software;
		std::cout << "Switched to Software Renderer\n";
	}
	else
	{
		m_RendererType = RendererType::Hardware;
		std::cout << "Switched to Hardware Renderer\n";
	}
}

void Renderer::ToggleRotation()
{
	m_IsRotating = !m_IsRotating;
}

void Renderer::CycleSamplerState()
{
	if (m_RendererType == RendererType::Hardware)
	{
		m_pHardwareRenderer->CycleSamplerState(m_Meshes);
	}
}

void dae::Renderer::CycleShadingMode()
{
	if (m_RendererType == RendererType::Software)
	{
		m_pSoftwareRenderer->CycleShadingMode();
	}
}

void dae::Renderer::ToggleNormalMap()
{
	if (m_RendererType == RendererType::Software)
	{
		m_pSoftwareRenderer->ToggleNormalMap();
	}
}

void dae::Renderer::ToggleDepthBufferVisualization()
{
	if (m_RendererType == RendererType::Software)
	{
		m_pSoftwareRenderer->ToggleDepthBufferVisualization();
	}
}

void Renderer::InitializeScene()
{
	//Initialize Camera
	m_Camera.Initialize(45.f, { .0f, .0f, 0.0f });
	m_Camera.farPlane = 100.f;
	const float aspectRatio = m_Width / static_cast<float>(m_Height);
	m_Camera.aspectRatio = aspectRatio;

	auto device = m_pHardwareRenderer->GetDevice();

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices);
	auto vehicleMesh = new Mesh(device, vertices, indices);

	m_pTexture = Texture::LoadFromFile("Resources/vehicle_diffuse.png", device);
	vehicleMesh->SetDiffuseMap(m_pTexture);
	m_pNormalMap = Texture::LoadFromFile("Resources/vehicle_normal.png", device);
	vehicleMesh->SetNormalMap(m_pNormalMap);
	m_pSpecularMap = Texture::LoadFromFile("Resources/vehicle_specular.png", device);
	vehicleMesh->SetSpecularMap(m_pSpecularMap);
	m_pGlossinessMap = Texture::LoadFromFile("Resources/vehicle_gloss.png", device);
	vehicleMesh->SetGlossinessMap(m_pGlossinessMap);

	vehicleMesh->m_WorldMatrix = Matrix::CreateTranslation(0.f, 0.f, 50.f);
	m_Meshes.emplace_back(vehicleMesh);

	vertices.clear();
	indices.clear();
	Utils::ParseOBJ("Resources/fireFX.obj", vertices, indices);
	Mesh* fireMesh = new Mesh(device, vertices, indices, ShaderType::Flat);

	m_pFireTexture = Texture::LoadFromFile("Resources/fireFX_diffuse.png", device);
	fireMesh->SetDiffuseMap(m_pFireTexture);

	fireMesh->m_WorldMatrix = Matrix::CreateTranslation(0.f, 0.f, 50.f);
	m_Meshes.emplace_back(fireMesh);
}