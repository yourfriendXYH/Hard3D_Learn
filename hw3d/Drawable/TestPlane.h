#pragma once
#include "../Drawable.h"

class TestPlane : public Drawable
{
public:
	TestPlane(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	DirectX::XMFLOAT3 m_pos;
	float m_roll = 0.0;
	float m_pitch = 0.0;
	float m_yaw = 0.0;
};

