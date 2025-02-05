#pragma once
#include "GraphicsResource.h"

class DepthStencil;

class RenderTarget : public GraphicsResource
{
public:
	RenderTarget(Graphics& gfx, UINT width, UINT height);

	void BindAsTexture(Graphics& gfx, UINT slot) const noexcept;

	void BindAsTarget(Graphics& gfx) const noexcept;

	void BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept;

	void Clear(Graphics& gfx, const std::array<float, 4>& color) const noexcept;

	void Clear(Graphics& gfx) const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTargetView;

	UINT m_width;
	UINT m_height;
};