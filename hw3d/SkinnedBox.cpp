//#include "SkinnedBox.h"
//#include "TransformCBuf.h"
//#include "IndexBuffer.h"
//#include "Cube.h"
//#include "Texture.h"
//#include "Surface.h"
//#include "VertexBuffer.h"
//#include "Sampler.h"
//#include "VertexShader.h"
//#include "PixelShader.h"
//#include "InputLayout.h"
//#include "Topology.h"
//
//SkinnedBox::SkinnedBox(Graphics& gfx, std::mt19937& rng,
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
//		// ���ɷ�����Ƭ������ģ������(Cube�а����������������)
//		auto model = Cube::MakeSkinned<Vertex>();
//
//		// ������ɫ������Դ�ӿ�
//		AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("CubeTexture.jpg")));
//		// �������㻺��
//		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.m_vertices));
//		// ��������
//		//AddStaticBind(std::make_unique<Sampler>(gfx));
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
//void SkinnedBox::Update(float delta) noexcept
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
//DirectX::XMMATRIX SkinnedBox::GetTransformXM() const noexcept
//{
//	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
//		DirectX::XMMatrixTranslation(m_radius, 0.0f, 0.0f) *
//		DirectX::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi) *
//		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
//}
