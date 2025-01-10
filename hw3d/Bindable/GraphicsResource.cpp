#include "GraphicsResource.h"

ID3D11DeviceContext* GraphicsResource::GetContext(Graphics& gfx) noexcept
{
	return gfx.m_pDeviceContext.Get();
}

ID3D11Device* GraphicsResource::GetDevice(Graphics& gfx) noexcept
{
	return gfx.m_pDevice.Get();
}

DxgiInfoManager& GraphicsResource::GetInfoManager(Graphics& gfx) noexcept
{
#ifndef NDEBUG
	return gfx.infoManager;
#endif
}