#pragma once
#include "../Graphics.h"
#include "GraphicsResource.h"

// 深度模板视口
class DepthStencil : public GraphicsResource
{
	friend class Graphics;
	friend class RenderTarget;
public:
	DepthStencil(Graphics& gfx, UINT width, UINT height);

	// 设置深度模板视图
	void BindAsDepthStencil(Graphics& gfx) const noexcept;

	// 清除深度模板视图
	void Clear(Graphics& gfx) const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
};