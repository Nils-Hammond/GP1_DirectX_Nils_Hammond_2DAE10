#pragma once
// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"

namespace dae
{
	struct Vector2;
	class Texture final
	{
	public:
		~Texture();

		static Texture* LoadFromFile(const std::string& path, ID3D11Device* pDevice);
		ColorRGB Sample(const Vector2& uv) const;
		ID3D11ShaderResourceView* GetSRV() const;

	private:
		Texture(ID3D11Texture2D* pTexture, ID3D11ShaderResourceView* pSRV, SDL_Surface* pSurface);

		// DirectX Resources
		ID3D11ShaderResourceView* m_pSRV{ nullptr };
		ID3D11Texture2D* m_pTexture{ nullptr };

		// Software Resources
		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };
	};
}