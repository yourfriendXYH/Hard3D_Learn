#pragma once
#include "../Graphics.h"
#include "GraphicsResource.h"

// ���ģ���ӿ�
class DepthStencil : public GraphicsResource
{
	friend class Graphics;
	friend class RenderTarget;
public:
	DepthStencil(Graphics& gfx, UINT width, UINT height);

	// �������ģ����ͼ
	void BindAsDepthStencil(Graphics& gfx) const noexcept;

	// ������ģ����ͼ
	void Clear(Graphics& gfx) const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
};