#include "RenderTarget.h"
#include "DepthStencil.h"
#include <array>

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
	GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexture);

	// 在纹理上创建资源视口
	D3D11_SHADER_RESOURCE_VIEW_DESC srcViewDesc = {};
	srcViewDesc.Format = textureDesc.Format;
	srcViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srcViewDesc.Texture2D.MostDetailedMip = 0;
	srcViewDesc.Texture2D.MipLevels = 1;
	GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srcViewDesc, &m_pTextureView);

	// 在纹理上创建目标视口
	D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc = {};
	rtViewDesc.Format = textureDesc.Format;
	rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtViewDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
	GetDevice(gfx)->CreateRenderTargetView(pTexture.Get(), &rtViewDesc, &m_pTargetView);
}

void RenderTarget::BindAsTexture(Graphics& gfx, UINT slot) const noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot, 1u, m_pTextureView.GetAddressOf());
}

void RenderTarget::BindAsTarget(Graphics& gfx) const noexcept
{
	GetContext(gfx)->OMSetRenderTargets(1u, m_pTargetView.GetAddressOf(), nullptr);
}

void RenderTarget::BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept
{
	GetContext(gfx)->OMSetRenderTargets(1u, m_pTargetView.GetAddressOf(), depthStencil.m_pDepthStencilView.Get());
}

void RenderTarget::Clear(Graphics& gfx, const std::array<float, 4>& color) const noexcept
{
	GetContext(gfx)->ClearRenderTargetView(m_pTargetView.Get(), color.data());
}

void RenderTarget::Clear(Graphics& gfx) const noexcept
{
	Clear(gfx, { 0.0f, 0.0f, 0.0f, 0.0f });
}
