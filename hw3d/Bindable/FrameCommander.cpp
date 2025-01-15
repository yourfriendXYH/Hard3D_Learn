#include "FrameCommander.h"
#include "Stencil.h"
#include "../ConstantBuffers.h"
#include "NullPixelShader.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "../InputLayout.h"
#include "../Sampler.h"
#include "../imgui/imgui.h"
#include "Blender.h"

FrameCommander::FrameCommander(Graphics& gfx)
	:
	m_depthStencil(gfx, gfx.GetWidth(), gfx.GetHeight()),
	m_renderTarget1({ gfx, gfx.GetWidth() / m_downFactor, gfx.GetHeight() / m_downFactor }),
	m_renderTarget2({ gfx, gfx.GetWidth() / m_downFactor, gfx.GetHeight() / m_downFactor }),
	m_blur(gfx, 7, 2.6f, "BlurOutline_PS.cso")
{

	// 创建全屏几何
	DynamicData::VertexLayout vtxLayout = MakeFullscreenQuadLayout();
	DynamicData::VerticesBuffer bufFull{ vtxLayout };
	bufFull.EmplaceBack(DirectX::XMFLOAT2{ -1, 1 });
	bufFull.EmplaceBack(DirectX::XMFLOAT2{ 1, 1 });
	bufFull.EmplaceBack(DirectX::XMFLOAT2{ -1, -1 });
	bufFull.EmplaceBack(DirectX::XMFLOAT2{ 1, -1 });
	m_pVbFull = Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull));
	std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
	m_pIbFull = Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

	// 创建全屏着色器
	m_pVsFull = Bind::VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
	m_pLayoutFull = Bind::InputLayout::Resolve(gfx, vtxLayout, m_pVsFull->GetByteCode());

	m_pSamplerFullPoint = Bind::Sampler::Resolve(gfx, false, true);
	m_pSamplerFullBilin = Bind::Sampler::Resolve(gfx, true, true);

	m_pBlenderMerge = Bind::Blender::Resolve(gfx, true);
}

void FrameCommander::Accept(Job job, size_t target) noexcept
{
	m_passes[target].Accept(job);
}

void FrameCommander::Execute(Graphics& gfx) noexcept
{
	using namespace Bind;

	m_depthStencil.Clear(gfx);
	m_renderTarget1->Clear(gfx);
	gfx.BindSwapBuffer(m_depthStencil);
	//m_renderTarget1.BindAsTarget(gfx, m_depthStencil);

	// main phong lighting pass
	Blender::Resolve(gfx, false)->Bind(gfx);
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	m_passes[0].Execute(gfx);

	//outline masking pass
	Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
	NullPixelShader::Resolve(gfx)->Bind(gfx);
	m_passes[1].Execute(gfx);

	// outline drawing pass
	m_renderTarget1->BindAsTarget(gfx);
	// 描边效果绘制到target1的texture上
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	m_passes[2].Execute(gfx);

	//// 绘制全屏纹理（后期处理）
	//gfx.BindSwapBuffer(m_depthStencil);
	//m_renderTarget1.BindAsTexture(gfx, 0);

	m_renderTarget2->BindAsTarget(gfx);
	m_renderTarget1->BindAsTexture(gfx, 0);
	m_pVbFull->Bind(gfx);
	m_pIbFull->Bind(gfx);
	m_pVsFull->Bind(gfx);
	m_pLayoutFull->Bind(gfx);
	m_pSamplerFullPoint->Bind(gfx);
	m_blur.Bind(gfx);
	m_blur.SetHorizontal(gfx);
	gfx.DrawIndexed(m_pIbFull->GetCount());

	gfx.BindSwapBuffer(m_depthStencil);
	m_renderTarget2->BindAsTexture(gfx, 0u);
	//m_pBlenderMerge->Bind(gfx);
	m_pSamplerFullBilin->Bind(gfx);

	Bind::Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
	m_blur.SetVertical(gfx);
	gfx.DrawIndexed(m_pIbFull->GetCount());

}

void FrameCommander::Reset() noexcept
{
	for (auto& pass : m_passes)
	{
		pass.Reset();
	}
}

void FrameCommander::BlurShowWindow(Graphics& gfx)
{
	if (ImGui::Begin("Blur"))
	{
		if (ImGui::SliderInt("Down Factor", &m_downFactor, 1, 16))
		{
			m_renderTarget1.emplace(gfx, gfx.GetWidth() / m_downFactor, gfx.GetHeight() / m_downFactor);
			m_renderTarget2.emplace(gfx, gfx.GetWidth() / m_downFactor, gfx.GetHeight() / m_downFactor);
		}
		m_blur.RenderWidget(gfx);
	}
	ImGui::End();
}
