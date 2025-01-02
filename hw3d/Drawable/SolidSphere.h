#pragma once
#include "../Drawable.h"

/// <summary>
/// «ÚÃÂÕ¯∏Ò¿‡
/// </summary>
class SolidSphere : public Drawable
{
public:
	SolidSphere(Graphics& gfx, float radius = 0.5f);

public:
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

public:
	void SetPos(const DirectX::XMFLOAT3& pos) noexcept;

private:
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
};