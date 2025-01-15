#pragma once
#include <array>
#include <memory>
#include "Pass.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "..\DynamicData\DynamicVertex.h"
#include "..\Effects\BlurPack.h"

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

	void Execute(Graphics& gfx) noexcept;

	// ������ж���
	void Reset() noexcept;

	void BlurShowWindow(Graphics& gfx);

private:
	// ������Ļ���εĶ��㲼��
	static DynamicData::VertexLayout MakeFullscreenQuadLayout()
	{
		DynamicData::VertexLayout vtxLayout;
		vtxLayout.Append(DynamicData::VertexLayout::Position2D);
		return vtxLayout;
	}

private:
	std::array<Pass, 3> m_passes;
	DepthStencil m_depthStencil;

	int m_downFactor = 1;

	std::optional<RenderTarget> m_renderTarget1;

	std::optional<RenderTarget> m_renderTarget2;

	BlurPack m_blur;

	// ���ڴ���Ĺ�������
	std::shared_ptr<Bind::VertexBuffer> m_pVbFull;
	std::shared_ptr<Bind::IndexBuffer> m_pIbFull;
	std::shared_ptr<Bind::VertexShader> m_pVsFull;
	//std::shared_ptr<Bind::PixelShader> m_pPsFull;
	std::shared_ptr<Bind::InputLayout> m_pLayoutFull;

	// �޸�ģ���Ŵ���ͨ���Ĳ�������
	std::shared_ptr<Bind::Sampler> m_pSamplerFullPoint;
	std::shared_ptr<Bind::Sampler> m_pSamplerFullBilin;

	std::shared_ptr<Bind::Blender> m_pBlenderMerge;
};