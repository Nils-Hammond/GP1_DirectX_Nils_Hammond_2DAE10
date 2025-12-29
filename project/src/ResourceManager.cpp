#define NOMINMAX
#include "ResourceManager.h"

dae::ResourceManager::ResourceManager(ID3D11Device* pDevice)
	: m_pDevice{ pDevice }, m_LoadedTextures{}
{
}

dae::ResourceManager::~ResourceManager()
{
}

dae::Texture* dae::ResourceManager::LoadTexture(const std::string& path)
{
	if (m_LoadedTextures.find(path) == m_LoadedTextures.end())
	{
		m_LoadedTextures.insert({ path, std::make_unique<Texture>(*Texture::LoadFromFile(path, m_pDevice)) });
	}
	return m_LoadedTextures.at(path).get();
}
