#include "Sampler.h"
#include "BindableCodex.h"

namespace Bind
{
	Sampler::Sampler(Graphics& gfx, bool anisoEnable, bool reflect)
		:
		m_anisoEnable(anisoEnable),
		m_reflect(reflect)
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		// samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.Filter = anisoEnable ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT; // �Ƿ����ø������Թ���
		samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY; // ���������Եȼ�
		// ��������mipmap����
		samplerDesc.MipLODBias = 0.0f; // LOD�����ƫ����
		samplerDesc.MinLOD = 0.0f; // LOD����С���𣨾�ϸ����
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // LOD����󼶱�ģ������

		GetDevice(gfx)->CreateSamplerState(&samplerDesc, &m_pSampler);
	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());
	}

	std::shared_ptr<Sampler> Bind::Sampler::Resolve(Graphics& gfx, bool anisoEnable, bool reflect)
	{
		return BindCodex::Resolve<Sampler>(gfx, anisoEnable, reflect);
	}

	std::string Sampler::GenerateUID(bool anisoEnable, bool reflect)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + (anisoEnable ? "A"s : "a"s) + (reflect ? "R"s : "W"s);
	}

	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID(m_anisoEnable, m_reflect);
	}


}


