//#include "Sheet.h"
//#include "TransformCBuf.h"
//#include "IndexBuffer.h"
//#include "Plane.h"
//#include "Texture.h"
//#include "Surface.h"
//#include "VertexBuffer.h"
//#include "Sampler.h"
//#include "VertexShader.h"
//#include "PixelShader.h"
//#include "InputLayout.h"
//#include "Topology.h"
//
//Sheet::Sheet(Graphics& gfx, std::mt19937& rng,
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
//				float u;
//				float v;
//			}tex;
//		};
//
//		// 生成方形面片的网格模型数据
//		auto model = Plane::Make<Vertex>();
//		// 给顶点数据赋纹理坐标的数据
//		model.m_vertices[0].tex = { 0.0f, 0.0f };
//		model.m_vertices[1].tex = { 1.0f, 0.0f };
//		model.m_vertices[2].tex = { 0.0f, 1.0f };
//		model.m_vertices[3].tex = { 1.0f, 1.0f };
//
//		// 创建着色器的资源视口
//		AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("pix_1.png")));
//		// 创建顶点缓存
//		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.m_vertices));
//		// 创建采样
//		AddStaticBind(std::make_unique<Sampler>(gfx));
//
//		auto pVertexShader = std::make_unique<VertexShader>(gfx, "TextureVS.cso");
//		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
//		AddStaticBind(std::move(pVertexShader));
//
//		AddStaticBind(std::make_unique<PixelShader>(gfx, "TexturePS.cso"));
//
//		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_indices));
//
//		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
//		{
//			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0 ,0 ,D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12 ,D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		};
//		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pVertexShaderByteCode));
//
//		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//	}
//	else
//	{
//		SetStaticIndexBuffer();
//	}
//
//	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
//}
//
//void Sheet::Update(float delta) noexcept
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
//DirectX::XMMATRIX Sheet::GetTransformXM() const noexcept
//{
//	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *	// 旋转矩阵 绕自身坐标旋转
//		DirectX::XMMatrixTranslation(m_radius, 0.f, 0.f) *	// 平移矩阵 朝相机的X轴偏移6
//		DirectX::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi) * // 绕相机旋转
//		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f); // 绕相机前方20处旋转
//}
