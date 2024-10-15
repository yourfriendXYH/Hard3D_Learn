#include "Sampler.h"
#include "BindableCodex.h"

namespace Bind
{
	Sampler::Sampler(Graphics& gfx)
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		// samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // 启用各向异性过滤
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY; // 最大各项异性等级
		// 采样器的mipmap设置
		samplerDesc.MipLODBias = 0.0f; // LOD级别的偏移量
		samplerDesc.MinLOD = 0.0f; // LOD的最小级别（精细纹理）
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // LOD的最大级别（模糊纹理）

		GetDevice(gfx)->CreateSamplerState(&samplerDesc, &m_pSampler);
	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());
	}

	std::shared_ptr<Sampler> Bind::Sampler::Resolve(Graphics& gfx)
	{
		return BindCodex::Resolve<Sampler>(gfx);
	}

	std::string Sampler::GenerateUID()
	{
		return typeid(Sampler).name();
	}

	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID();
	}


}


