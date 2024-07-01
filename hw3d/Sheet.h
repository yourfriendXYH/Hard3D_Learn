//#pragma once
//#include "DrawableBase.h"
//#include <random>
//
//class Sheet : public DrawableBase<Sheet>
//{
//public:
//	Sheet(Graphics& gfx, std::mt19937& rng,
//		std::uniform_real_distribution<float>& adist,
//		std::uniform_real_distribution<float>& ddist,
//		std::uniform_real_distribution<float>& odist,
//		std::uniform_real_distribution<float>& rdist);
//
//	virtual void Update(float delta) noexcept override;
//
//	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
//
//private:
//	// positional
//	float m_radius = 0.f;
//	float m_roll = 0.f;
//	float m_pitch = 0.f;
//	float m_yaw = 0.f;
//
//	float m_theta; //
//	float m_phi; // ¦Õ
//	float m_chi; // ¦Ö
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