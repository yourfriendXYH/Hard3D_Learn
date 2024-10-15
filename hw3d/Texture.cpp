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

		m_hasAlpha = surface.AlphaLoaded();

		// 2d纹理的描述
		D3D11_TEXTURE2D_DESC texture2dDesc = {};
		texture2dDesc.Width = surface.GetWidth();
		texture2dDesc.Height = surface.GetHeight();
		texture2dDesc.MipLevels = 0; // 设置为0，以生成所有Mip级别
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // 使用mipmap必须要加上 D3D11_BIND_RENDER_TARGET
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 设置这个标志来允许使用 ID3D11DeviceContext::GenerateMips 方法自动生成 Mip 贴图

		//D3D11_SUBRESOURCE_DATA sd = {};
		//sd.pSysMem = surface.GetBufferPtr();
		//sd.SysMemPitch = surface.GetWidth() * sizeof(Surface::Color);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2d;
		GetDevice(gfx)->CreateTexture2D(&texture2dDesc, /*&sd*/nullptr, &pTexture2d);

		// 用于更新资源数据。这通常用于将新的数据复制到资源（例如纹理或缓冲区）中
		// 此方法只能用于不能直接映射的资源，这些资源通常使用 D3D11_USAGE_DYNAMIC 或 D3D11_USAGE_DEFAULT 作为使用模式
		GetContext(gfx)->UpdateSubresource(pTexture2d.Get(), 0u, nullptr, surface.GetBufferPtr(), surface.GetWidth() * sizeof(Surface::Color), 0u);

		D3D11_SHADER_RESOURCE_VIEW_DESC srcViewDesc = {};
		srcViewDesc.Format = texture2dDesc.Format;
		srcViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srcViewDesc.Texture2D.MostDetailedMip = 0;
		srcViewDesc.Texture2D.MipLevels = -1; // 设置为 -1（即 ~0U），表示使用从 MostDetailedMip 开始的所有 Mip 级别
		GetDevice(gfx)->CreateShaderResourceView(pTexture2d.Get(), &srcViewDesc, &m_pTextureView);

		if (nullptr != m_pTextureView)
		{
			GetContext(gfx)->GenerateMips(m_pTextureView.Get()); // ???
		}
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

	UINT Texture::CalculateNumberOfMipLevels(UINT width, UINT height) noexcept
	{
		const float xSteps = std::ceil(log2((float)width));
		const float ySteps = std::ceil(log2((float)height));
		return (UINT)std::max(xSteps, ySteps);
	}

}


