#include "RenderTarget.h"

RenderTarget::RenderTarget(Graphics& gfx, UINT width, UINT height)
{
	// 创建纹理资源
	D3D11_TEXTURE2D_DESC  textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1u;
	textureDesc.ArraySize = 1u;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

}

void RenderTarget::BindAsTexture(Graphics& gfx, UINT slot) const noexcept
{

}

void RenderTarget::BindAsTarget(Graphics& gfx) const noexcept
{

}

void RenderTarget::BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept
{

}
