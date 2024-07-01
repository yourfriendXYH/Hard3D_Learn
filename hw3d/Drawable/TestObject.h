//#pragma once
//#include "../DrawableBase.h"
//#include <random>
//
//// 将通用的变量提取出来
//template <class T>
//class TestObject : public DrawableBase<T>
//{
//public:
//	TestObject(Graphics& gfx, std::mt19937& rng,
//		std::uniform_real_distribution<float>& adist,
//		std::uniform_real_distribution<float>& ddist,
//		std::uniform_real_distribution<float>& odist,
//		std::uniform_real_distribution<float>& rdist)
//		:
//		m_radius(rdist(rng)),
//		m_dRoll(ddist(rng)),
//		m_dPitch(ddist(rng)),
//		m_dYaw(ddist(rng)),
//		m_dTheta(odist(rng)),
//		m_dPhi(odist(rng)),
//		m_dChi(odist(rng)),
//		m_theta(adist(rng)),
//		m_phi(adist(rng)),
//		m_chi(adist(rng))
//	{
//	}
//
//	// 每帧更新变换值
//	void Update(float delta) noexcept override
//	{
//		m_roll += m_dRoll * delta;
//		m_pitch += m_dPitch * delta;
//		m_yaw += m_dYaw * delta;
//
//		m_theta += m_dTheta * delta;
//		m_phi += m_dPhi * delta;
//		m_chi += m_dChi * delta;
//	}
//
//	// 物体的变换矩阵
//	DirectX::XMMATRIX GetTransformXM() const noexcept override
//	{
//		return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *	// 旋转矩阵 绕自身坐标旋转
//			DirectX::XMMatrixTranslation(m_radius, 0.f, 0.f) *	// 平移矩阵 朝相机的X轴偏移6
//			DirectX::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi) * // 绕相机旋转
//			DirectX::XMMatrixTranslation(5.f, 5.f, 15.f); // 绕相机前方20处旋转
//	}
//private:
//	// positional
//	float m_radius = 0.f;
//	float m_roll = 0.f;
//	float m_pitch = 0.f;
//	float m_yaw = 0.f;
//
//	float m_theta; //
//	float m_phi; // φ
//	float m_chi; // χ
//
//	// speed (delta/s)
//	float m_dRoll;
//	float m_dPitch;
//	float m_dYaw;
//
//	float m_dTheta;
//	float m_dPhi;
//	float m_dChi;
//};