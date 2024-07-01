#include "Bindable.h"

ID3D11DeviceContext* Bindable::GetContext(Graphics& gfx) noexcept
{
	return gfx.m_pDeviceContext.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics& gfx) noexcept
{
	return gfx.m_pDevice.Get();
}

DxgiInfoManager& Bindable::GetInfoManager(Graphics& gfx) noexcept
{
#ifndef NDEBUG
	return gfx.infoManager;
#endif
}

