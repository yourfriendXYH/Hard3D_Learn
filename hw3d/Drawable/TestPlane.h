#pragma once
#include "../Drawable.h"

class TestPlane : public Drawable
{
public:
	TestPlane(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void SpawnControlWindow(Graphics& gfx) noexcept;

private:
	DirectX::XMFLOAT3 m_pos = { 0.0f, 0.0f, 0.0f };
	float m_roll = 0.0;
	float m_pitch = 0.0;
	float m_yaw = 0.0;

	// 着色器常量
	struct PSMaterialConstant
	{
		float m_specularIntensity = 0.18f;
		float m_specularPower = 18.0f;
		BOOL m_normalMappingEnabled = TRUE;
		float padding[1]; // 内存对齐（四字节对齐）
	} m_pmc;
};

