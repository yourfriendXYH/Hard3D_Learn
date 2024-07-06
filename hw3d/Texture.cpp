#include "Texture.h"
#include "Surface.h"
#include "BindableCodex.h"

namespace Bind
{
	Texture::Texture(Graphics& gfx, const std::string& path, UINT slot)
		:
		m_slot(slot),
		m_path(path)
	{
		// 获取纹理数据
		const auto surface = Surface::FromFile(path);

		//m_hasAlpha = surface.AlphaLoaded();

		// 2d纹理的描述
		D3D11_TEXTURE2D_DESC texture2dDesc = {};
		texture2dDesc.Width = surface.GetWidth();
		texture2dDesc.Height = surface.GetHeight();
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = surface.GetBufferPtr();
		sd.SysMemPitch = surface.GetWidth() * sizeof(Surface::Color);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2d;
		GetDevice(gfx)->CreateTexture2D(&texture2dDesc, &sd, &pTexture2d);

		D3D11_SHADER_RESOURCE_VIEW_DESC srcViewDesc = {};
		srcViewDesc.Format = texture2dDesc.Format;
		srcViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srcViewDesc.Texture2D.MostDetailedMip = 0;
		srcViewDesc.Texture2D.MipLevels = 1;
		GetDevice(gfx)->CreateShaderResourceView(pTexture2d.Get(), &srcViewDesc, &m_pTextureView);

	}

	void Texture::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(m_slot, 1, m_pTextureView.GetAddressOf());
	}

	std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot)
	{
		return BindCodex::Resolve<Texture>(gfx, path, slot);
	}

	std::string Texture::GenerateUID(const std::string& path, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#" + std::to_string(slot);
	}

	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID(m_path, m_slot);
	}

	bool Texture::HasAlpha() const noexcept
	{
		return m_hasAlpha;
	}
}


