#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera() noexcept;

public:
	// 相机矩阵
	DirectX::XMMATRIX GetMatrix() const noexcept;
	// 
	void SpawnControlWindow() noexcept;

	void Reset() noexcept;

	// 旋转
	void Rotate(float dx, float dy) noexcept;
	// 平移
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

	DirectX::XMFLOAT3 GetPos() const noexcept;

private:
	DirectX::XMFLOAT3 m_pos; // 位置
	float m_pitch; // 上下旋转
	float m_yaw; // 左右旋转
	static constexpr float m_travelSpeed = 12.0f; // 平移速度
	static constexpr float m_rotationSpeed = 0.004f; // 旋转速度
};