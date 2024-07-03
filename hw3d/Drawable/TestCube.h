#pragma once
#include "../Drawable.h"

class TestCube : public Drawable
{
public:
	TestCube(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void SpawnControlWindow(Graphics& gfx) noexcept;

private:
	// 像素着色器常量缓存
	struct PSMaterialConstant
	{
		float specularIntensity = 0.1f;
		float specularPower = 20.0f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];
	}m_pmc;

	DirectX::XMFLOAT3 m_pos = { 1.0f, 1.0f, 1.0f };
	float m_roll = 0.f;
	float m_pitch = 0.f;
	float m_yaw = 0.f;
};