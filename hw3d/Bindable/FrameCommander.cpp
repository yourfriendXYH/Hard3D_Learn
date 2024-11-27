#include "FrameCommander.h"
#include "Stencil.h"
#include "../ConstantBuffers.h"
#include "NullPixelShader.h"

void FrameCommander::Accept(Job job, size_t target) noexcept
{
	m_passes[target].Accept(job);
}

void FrameCommander::Execute(Graphics& gfx) const noexcept
{
	using namespace Bind;
	// main phong lighting pass
	Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Off)->Bind(gfx);
	m_passes[0].Execute(gfx);
	// outline masking pass
	Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
	NullPixelShader::Resolve(gfx)->Bind(gfx);
	m_passes[1].Execute(gfx);
	// outline drawing pass
	Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
	struct SolidColorBuffer
	{
		DirectX::XMFLOAT4 color = { 1.0f,0.4f,0.4f,1.0f };
	} scb;
	PixelConstantBuffer<SolidColorBuffer>::Resolve(gfx, scb, 1u)->Bind(gfx);
	m_passes[2].Execute(gfx);
}

void FrameCommander::Reset() noexcept
{
	for (auto& pass : m_passes)
	{
		pass.Reset();
	}
}
