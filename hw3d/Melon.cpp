//#include "Melon.h"
//#include "VertexShader.h"
//#include "PixelShader.h"
//#include "ConstantBuffers.h"
//#include "InputLayout.h"
//#include "Topology.h"
//#include "Sphere.h"
//#include "VertexBuffer.h"
//#include "IndexBuffer.h"
//#include "TransformCBuf.h"
//
//
//Melon::Melon(Graphics& gfx, std::mt19937& rng,
//	std::uniform_real_distribution<float>& adist,
//	std::uniform_real_distribution<float>& ddist,
//	std::uniform_real_distribution<float>& odist,
//	std::uniform_real_distribution<float>& rdist,
//	std::uniform_int_distribution<int>& longdist,
//	std::uniform_int_distribution<int>& latdist)
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
//		auto pVertexShader = std::make_unique<VertexShader>(gfx, "VertexShader.cso");
//		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
//
//		AddStaticBind(std::move(pVertexShader));
//
//		AddStaticBind(std::make_unique<PixelShader>(gfx, "PixelShader.cso"));
//
//
//		struct PixelShaderConstants
//		{
//			struct
//			{
//				float r;
//				float g;
//				float b;
//				float a;
//			} face_colors[8];
//		};
//		const PixelShaderConstants psConstant =
//		{
//			{
//				{ 1.0f,1.0f,1.0f },
//				{ 1.0f,0.0f,0.0f },
//				{ 0.0f,1.0f,0.0f },
//				{ 1.0f,1.0f,0.0f },
//				{ 0.0f,0.0f,1.0f },
//				{ 1.0f,0.0f,1.0f },
//				{ 0.0f,1.0f,1.0f },
//				{ 0.0f,0.0f,0.0f },
//			}
//		};
//
//		AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, psConstant));
//
//		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
//		{
//			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		};
//		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pVertexShaderByteCode));
//
//		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//	}
//
//	// 经纬线随机导致顶点缓存和索引缓存可变，所以不能static复用
//	struct Vertex
//	{
//		DirectX::XMFLOAT3 pos;
//	};
//
//	IndexedTriangleList<Vertex> model = Sphere::MakeTesselated<Vertex>(latdist(rng), longdist(rng));
//	model.Transform(DirectX::XMMatrixScaling(1.f, 1.f, 1.2f));
//
//	AddBind(std::make_unique<VertexBuffer>(gfx, model.m_vertices));
//
//	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_indices));
//
//	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
//}
//
//void Melon::Update(float delta) noexcept
//{
//	m_roll += m_dRoll * delta;
//	m_pitch += m_dPitch * delta;
//	m_yaw += m_dYaw * delta;
//
//	m_theta += m_dTheta * delta;
//	m_phi += m_dPhi * delta;
//	m_chi += m_dChi * delta;
//}
//
//DirectX::XMMATRIX Melon::GetTransformXM() const noexcept
//{
//	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *	// 旋转矩阵 绕自身坐标旋转
//		DirectX::XMMatrixTranslation(m_radius, 0.f, 0.f) *	// 平移矩阵 朝相机的X轴偏移6
//		DirectX::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi) * // 绕相机旋转
//		DirectX::XMMatrixTranslation(5.f, 5.f, 30.f); // 绕相机前方20处旋转;
//}
//
