#pragma once
#include "../Drawable.h"

class TestPlane : public Drawable
{
public:
	TestPlane(Graphics& gfx, float size, DirectX::XMFLOAT4 color);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void SpawnControlWindow(Graphics& gfx, const std::string& titleName) noexcept;

private:
	DirectX::XMFLOAT3 m_pos = { 0.0f, 0.0f, 0.0f };
	float m_roll = 0.0;
	float m_pitch = 0.0;
	float m_yaw = 0.0;

	// 着色器常量
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT4 color;
	} m_pmc;
};

