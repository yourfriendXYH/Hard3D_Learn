#pragma once
#include <array>
#include "Pass.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "..\DynamicData\DynamicVertex.h"
#include <memory>

namespace Bind
{
	class VertexBuffer;
	class IndexBuffer;
	class VertexShader;
	class PixelShader;
	class InputLayout;
	class Sampler;
	class Blender;
}

class FrameCommander
{
public:
	FrameCommander(Graphics& gfx);

	void Accept(Job job, size_t target) noexcept;

	void Execute(Graphics& gfx) const noexcept;

	// 清空所有队列
	void Reset() noexcept;

private:
	// 创建屏幕几何的顶点布局
	static DynamicData::VertexLayout MakeFullscreenQuadLayout()
	{
		DynamicData::VertexLayout vtxLayout;
		vtxLayout.Append(DynamicData::VertexLayout::Position2D);
		return vtxLayout;
	}

private:
	std::array<Pass, 3> m_passes;
	DepthStencil m_depthStencil;

	// 后期处理的管线数据
	RenderTarget m_renderTarget;
	std::shared_ptr<Bind::VertexBuffer> m_pVbFull;
	std::shared_ptr<Bind::IndexBuffer> m_pIbFull;
	std::shared_ptr<Bind::VertexShader> m_pVsFull;
	std::shared_ptr<Bind::PixelShader> m_pPsFull;
	std::shared_ptr<Bind::InputLayout> m_pLayoutFull;
	std::shared_ptr<Bind::Sampler> m_pSamplerFull;
	std::shared_ptr<Bind::Blender> m_pBlenderFull;
};