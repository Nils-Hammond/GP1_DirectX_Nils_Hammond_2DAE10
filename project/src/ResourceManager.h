#pragma once
// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

#include <map>
#include <memory>
#include "Texture.h"

namespace dae
{
	class ResourceManager final
	{
	public:
		ResourceManager(ID3D11Device* pDevice);
		~ResourceManager();
		Texture* LoadTexture(const std::string& path);
	private:
		std::map<std::string, std::unique_ptr<Texture>> m_LoadedTextures{};
		ID3D11Device* m_pDevice{};
	};
};