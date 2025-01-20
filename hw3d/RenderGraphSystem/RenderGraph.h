#pragma once
#include <vector>
#include <memory>

class Graphics;
class Pass;
class PassOutput;
class PassInput;
class RenderTarget;
class DepthStencil;

class RenderGraph
{
public:
	RenderGraph(Graphics& gfx);
	~RenderGraph();

protected:
private:

	std::vector<std::unique_ptr<Pass>> m_passPtrs;

	std::vector<std::unique_ptr<PassOutput>> m_globalSources;

	std::vector<std::unique_ptr<PassInput>> m_globalSinks;

	std::shared_ptr<RenderTarget> m_pBackBufferTarget;

	// �����ģ����ͼ
	std::shared_ptr<DepthStencil> m_pMasterDepth;

	// �Ƿ������
	bool m_finalized = false;
};