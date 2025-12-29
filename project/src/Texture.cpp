#define NOMINMAX
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>
#include <iostream>

namespace dae
{
	Texture::Texture(ID3D11Texture2D* pTexture, ID3D11ShaderResourceView* pSRV, SDL_Surface* pSurface)
	: m_pTexture(pTexture)
		, m_pSRV(pSRV),
		m_pSurface(pSurface),
		m_pSurfacePixels(static_cast<uint32_t*>(pSurface->pixels))
	{
	}

	Texture::~Texture()
	{
		if (m_pSRV) m_pSRV->Release();
		if (m_pTexture) m_pTexture->Release();
		if (m_pSurface) SDL_FreeSurface(m_pSurface);
	}

	Texture* Texture::LoadFromFile(const std::string& path, ID3D11Device* pDevice)
	{
		//TODO
		//Load SDL_Surface using IMG_LOAD
		//Create & Return a new Texture Object (using SDL_Surface)


		SDL_Surface* pSurface = IMG_Load(path.c_str());
		if (!pSurface)
		{
			std::cerr << "Failed to load texture: " << path << "\n";
			return nullptr;
		}

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pSurface->w;
		desc.Height = pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);


		ID3D11Texture2D* pTexture{};
		HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &pTexture);
		if (FAILED(hr))
		{
			std::cerr << "Failed to create texture2D for: " << path << "\n";
			pTexture->Release();
			SDL_FreeSurface(pSurface);
			return nullptr;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		ID3D11ShaderResourceView* pSRV{};
		
		hr = pDevice->CreateShaderResourceView(pTexture, &SRVDesc, &pSRV);
		if (FAILED(hr))
		{
			std::cerr << "Failed to create shader resource view for: " << path << "\n";
			pSRV->Release();
			pTexture->Release();
			return nullptr;
		}

		return new Texture(pTexture, pSRV, pSurface);
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		int u = static_cast<int>(uv.x * m_pSurface->w) % m_pSurface->w;
		int v = static_cast<int>(uv.y * m_pSurface->h) % m_pSurface->h;

		if (u < 0) u += m_pSurface->w;
		if (v < 0) v += m_pSurface->h;
		uint32_t pixel = m_pSurfacePixels[u + v * m_pSurface->w];
		uint8_t r, g, b;
		SDL_GetRGB(pixel, m_pSurface->format, &r, &g, &b);
		ColorRGB color{ r / 255.f, g / 255.f, b / 255.f };
		return color;
		return ColorRGB{};
	}

	ID3D11ShaderResourceView* Texture::GetSRV() const
	{
		if (m_pSRV)
			return m_pSRV;
		return nullptr;
	}
}