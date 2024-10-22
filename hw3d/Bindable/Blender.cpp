#include "Blender.h"
#include "../BindableCodex.h"

namespace Bind
{

	Blender::Blender(Graphics& gfx, bool blending)
		: m_blending(blending)
	{
		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
		auto& blendRenderTarget = blendDesc.RenderTarget[0];
		if (m_blending)
		{
			blendRenderTarget.BlendEnable = TRUE;
			blendRenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendRenderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendRenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
			blendRenderTarget.SrcBlendAlpha = D3D11_BLEND_ZERO;
			blendRenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
			blendRenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendRenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		// ´´½¨»ìºÏ×´Ì¬
		GetDevice(gfx)->CreateBlendState(&blendDesc, &m_pBlender);
	}

	void Blender::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->OMSetBlendState(m_pBlender.Get(), nullptr, 0xFFFFFFFFu);
	}

	std::shared_ptr<Blender> Blender::Resolve(Graphics& gfx, bool blending)
	{
		return BindCodex::Resolve<Blender>(gfx, blending);
	}

	std::string Blender::GenerateUID(bool blending)
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (blending ? "b" : "n");
	}

	std::string Blender::GetUID() const noexcept
	{
		return GenerateUID(m_blending);
	}

}