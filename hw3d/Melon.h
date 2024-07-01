//#pragma once
//#include "DrawableBase.h"
//#include <random>
//
//class Melon : public DrawableBase<Melon>
//{
//public:
//	Melon(Graphics& gfx, std::mt19937& rng,
//		std::uniform_real_distribution<float>& adist,
//		std::uniform_real_distribution<float>& ddist,
//		std::uniform_real_distribution<float>& odist,
//		std::uniform_real_distribution<float>& rdist,
//		std::uniform_int_distribution<int>& longdist,
//		std::uniform_int_distribution<int>& latdist);
//
//	void Update(float delta) noexcept override;
//
//	DirectX::XMMATRIX GetTransformXM()const noexcept override;
//
//private:
//
//	// ¾àÀëÏà»úµÄ°ë¾¶
//	float m_radius;
//
//	float m_roll = 0.f;
//	float m_pitch = 0.f;
//	float m_yaw = 0.f;
//
//	float m_dRoll;
//	float m_dPitch;
//	float m_dYaw;
//
//	float m_theta;
//	float m_phi;
//	float m_chi;
//
//	float m_dTheta;
//	float m_dPhi;
//	float m_dChi;
//
//};