#include "Rasterizer.h"
#include "../BindableCodex.h"

namespace Bind
{
	Rasterizer::Rasterizer(Graphics& gfx, bool twoSided)
		:
		m_twoSided(twoSided)
	{
		// Ĭ������
		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.CullMode = m_twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

		GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &m_pRasterizer);
	}

	void Rasterizer::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->RSSetState(m_pRasterizer.Get());
	}

	std::string Rasterizer::GetUID() const noexcept
	{
		return GenerateUID(m_twoSided);
	}

	std::shared_ptr<Rasterizer> Rasterizer::Resolve(Graphics& gfx, bool twoSided)
	{
		return BindCodex::Resolve<Rasterizer>(gfx, twoSided);
	}

	std::string Rasterizer::GenerateUID(bool twoSided)
	{
		using namespace std::string_literals;
		return typeid(Rasterizer).name() + "#"s + (twoSided ? "2s" : "1s");
	}
}