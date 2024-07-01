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
	DirectX::XMFLOAT3 m_pos;
	float m_roll = 0.0;
	float m_pitch = 0.0;
	float m_yaw = 0.0;

	// ��ɫ������
	struct PSMaterialConstant
	{
		float m_specularIntensity = 0.1f;
		float m_specularPower = 20.0f;
		BOOL m_normalMappingEnabled = TRUE;
		float padding[1]; // �ڴ���루���ֽڶ��룩
	} m_pmc;
};

