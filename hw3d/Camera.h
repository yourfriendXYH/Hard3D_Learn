#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera() noexcept;

public:
	// �������
	DirectX::XMMATRIX GetMatrix() const noexcept;
	// 
	void SpawnControlWindow() noexcept;

	void Reset() noexcept;

	// ��ת
	void Rotate(float dx, float dy) noexcept;
	// ƽ��
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

	DirectX::XMFLOAT3 GetPos() const noexcept;

private:
	DirectX::XMFLOAT3 m_pos; // λ��
	float m_pitch; // ������ת
	float m_yaw; // ������ת
	static constexpr float m_travelSpeed = 12.0f; // ƽ���ٶ�
	static constexpr float m_rotationSpeed = 0.004f; // ��ת�ٶ�
};