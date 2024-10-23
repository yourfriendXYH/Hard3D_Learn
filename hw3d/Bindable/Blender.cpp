#include "Blender.h"
#include "../BindableCodex.h"

namespace Bind
{

	Blender::Blender(Graphics& gfx, bool blending, std::optional<float> factor_in)
		: m_blending(blending)
	{
		if (factor_in)
		{
			m_factors.emplace();
			m_factors->fill(*factor_in);
		}

		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
		auto& blendRenderTarget = blendDesc.RenderTarget[0];
		if (m_blending)
		{
			blendRenderTarget.BlendEnable = TRUE;
			//blendRenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			//blendRenderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			//blendRenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
			//blendRenderTarget.SrcBlendAlpha = D3D11_BLEND_ZERO;
			//blendRenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
			//blendRenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			//blendRenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			if (factor_in) // 有混合因子
			{
				blendRenderTarget.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				blendRenderTarget.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else // 
			{
				blendRenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendRenderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
		}
		// 创建混合状态
		GetDevice(gfx)->CreateBlendState(&blendDesc, &m_pBlender);
	}

	void Blender::Bind(Graphics& gfx) noexcept
	{
		const auto* data = m_factors ? m_factors->data() : nullptr;
		GetContext(gfx)->OMSetBlendState(m_pBlender.Get(), data, 0xFFFFFFFFu);
	}

	std::shared_ptr<Blender> Blender::Resolve(Graphics& gfx, bool blending, std::optional<float> factor)
	{
		return BindCodex::Resolve<Blender>(gfx, blending, factor);
	}

	std::string Blender::GenerateUID(bool blending, std::optional<float> factor)
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (blending ? "b"s : "n"s) + (factor ? "#f"s + std::to_string(*factor) : "");
	}

	std::string Blender::GetUID() const noexcept
	{
		return GenerateUID(m_blending, m_factors ? m_factors->front() : std::optional<float>{});
	}

	void Blender::SetFactor(float factor) noexcept
	{
		assert(m_factors);
		m_factors->fill(factor);
	}

	float Blender::GetFactor() const noexcept
	{
		assert(m_factors);
		return m_factors->front();
	}

}