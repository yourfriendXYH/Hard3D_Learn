//#pragma once
//#include "../DrawableBase.h"
//#include <random>
//
//// ��ͨ�õı�����ȡ����
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
//	// ÿ֡���±任ֵ
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
//	// ����ı任����
//	DirectX::XMMATRIX GetTransformXM() const noexcept override
//	{
//		return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *	// ��ת���� ������������ת
//			DirectX::XMMatrixTranslation(m_radius, 0.f, 0.f) *	// ƽ�ƾ��� �������X��ƫ��6
//			DirectX::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi) * // �������ת
//			DirectX::XMMatrixTranslation(5.f, 5.f, 15.f); // �����ǰ��20����ת
//	}
//private:
//	// positional
//	float m_radius = 0.f;
//	float m_roll = 0.f;
//	float m_pitch = 0.f;
//	float m_yaw = 0.f;
//
//	float m_theta; //
//	float m_phi; // ��
//	float m_chi; // ��
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