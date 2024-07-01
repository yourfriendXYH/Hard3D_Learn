//#include "Pyramid.h"
//
//Pyramid::Pyramid(Graphics& gfx, std::mt19937& rng,
//	std::uniform_real_distribution<float>& adist,
//	std::uniform_real_distribution<float>& ddist,
//	std::uniform_real_distribution<float>& odist,
//	std::uniform_real_distribution<float>& rdist)
//	:
//	m_radius(rdist(rng)),
//	m_dRoll(ddist(rng)),
//	m_dPitch(ddist(rng)),
//	m_dYaw(ddist(rng)),
//	m_dTheta(odist(rng)),
//	m_dPhi(odist(rng)),
//	m_dChi(odist(rng)),
//	m_theta(adist(rng)),
//	m_phi(adist(rng)),
//	m_chi(adist(rng))
//{
//	if (!IsStaticInitialized())
//	{
//		struct Vertex
//		{
//			DirectX::XMFLOAT3 pos;
//			struct
//			{
//				unsigned char r;
//				unsigned char g;
//				unsigned char b;
//				unsigned char a;
//			} color;
//		};
//	}
//	else
//	{
//
//	}
//}
//
//void Pyramid::Update(float dt) noexcept
//{
//
//}
//
//DirectX::XMMATRIX Pyramid::GetTransformXM() const noexcept
//{
//	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *	// 旋转矩阵 绕自身坐标旋转
//		DirectX::XMMatrixTranslation(m_radius, 0.f, 0.f) *	// 平移矩阵 朝相机的X轴偏移6
//		DirectX::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi) * // 绕相机旋转
//		DirectX::XMMatrixTranslation(5.f, 5.f, 30.f); // 绕相机前方20处旋转
//}
