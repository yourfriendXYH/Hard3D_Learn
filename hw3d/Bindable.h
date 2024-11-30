#pragma once

#include "Graphics.h"

class Drawable;

class Bindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual std::string GetUID() const noexcept
	{
		assert(false);
		return "";
	}
	virtual ~Bindable() = default;
public:
	virtual void InitializeParentReference(const Drawable& parent) noexcept {};

	virtual void Accept(class TechniqueProbe& probe) {};

protected:
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(Graphics& gfx) noexcept;

private:
};