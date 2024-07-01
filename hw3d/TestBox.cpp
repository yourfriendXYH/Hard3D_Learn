//#include "TestBox.h"
//#include "VertexBuffer.h"
//#include "VertexShader.h"
//#include "PixelShader.h"
//#include "ConstantBuffers.h"
//#include "InputLayout.h"
//#include "Topology.h"
//#include "TransformCBuf.h"
//#include "IndexBuffer.h"
//#include "Sphere.h"
//#include "Prism.h"
//#include "Cube.h"
//
//TestBox::TestBox(Graphics& gfx, std::mt19937& rng,
//	std::uniform_real_distribution<float>& adist,
//	std::uniform_real_distribution<float>& ddist,
//	std::uniform_real_distribution<float>& odist,
//	std::uniform_real_distribution<float>& rdist,
//	std::uniform_real_distribution<float>& bdist,
//	DirectX::XMFLOAT3 material)
//	:
//	TestObject(gfx, rng, adist, ddist, odist, rdist)
//{
//	if (!IsStaticInitialized())
//	{
//		// ����ṹ
//		struct Vertex
//		{
//			DirectX::XMFLOAT3 pos;
//			DirectX::XMFLOAT3 normal;
//		};
//
//		// ����Cube��ģ������
//		auto model = Cube::MakeIndependent<Vertex>(); // ������
//		model.SetNormalsIndependentFlat();
//		//model.Transform(DirectX::XMMatrixScaling(1.f, 1.f, 1.2f));
//
//		//const std::vector<Vertex> vertices =
//		//{
//		//	{-1.f, -1.f, -1.f},
//		//	{1.f, -1.f, -1.f},
//		//	{-1.f, 1.f, -1.f},
//		//	{1.f, 1.f, -1.f},
//		//	{-1.f, -1.f, 1.f},
//		//	{1.f, -1.f, 1.f},
//		//	{-1.f, 1.f, 1.f},
//		//	{1.f, 1.f, 1.f},
//		//};
//
//		// �������㻺�棨����װ��׶Σ�
//		//AddBind(std::make_unique<VertexBuffer>(gfx, vertices));
//		//AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));
//		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.m_vertices));
//
//		// ����������ɫ��
//		auto pVertexShader = std::make_unique<VertexShader>(gfx, "PhongVS.cso");
//		auto pVertexShaderBtyeCode = pVertexShader->GetByteCode();
//		// ��������ɫ���׶Σ�
//		//AddBind(std::move(pVertexShader));
//		AddStaticBind(std::move(pVertexShader));
//		// ���������ɫ����������ɫ���׶Σ�
//		//AddBind(std::make_unique<PixelShader>(gfx, "PixelShader.cso"));
//		AddStaticBind(std::make_unique<PixelShader>(gfx, "PhongPS.cso"));
//
//		// ����������棨������ɫ���׶Σ�
//		// AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
//		//AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
//		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_indices));
//
//		// ������벼�֣�����װ��׶Σ�
//		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
//		{
//			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//			{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		};
//		//AddBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pVertexShaderBtyeCode)); // ��Ҫ���붥����ɫ�����ֽ���
//		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pVertexShaderBtyeCode)); // ��Ҫ���붥����ɫ�����ֽ���
//
//		// ��ӻ�Ԫ���ˣ�����װ��׶Σ�
//		//AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//	}
//	else
//	{
//		// ����һ���⣬����drawable������������Ҫ��ǰ��ĸ�ֵ���������治�ܸ�Ϊstatic��������ݶ��㻺��������仯
//		SetStaticIndexBuffer();
//	}
//
//	// ��ӳ�ʼ�ı任��������ɫ���׶Σ�
//	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
//
//	// Cube��ɫ�ĳ�������
//	struct PSMaterialConstant
//	{
//		alignas(16) DirectX::XMFLOAT3 m_color; // 16�ֽڶ���
//		float specularIntensity = 0.6f; // ����߹�ǿ��
//		float specularPower = 30.f; // ����߹⹦��
//		float m_padding[2]; // ��������
//	} colorConstant;
//	colorConstant.m_color = material;
//	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConstant, 1u));
//
//	// ���������ž���
//	DirectX::XMStoreFloat3x3(&m_mt, DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f));
//}
//
//DirectX::XMMATRIX TestBox::GetTransformXM() const noexcept
//{
//	return DirectX::XMLoadFloat3x3(&m_mt) * TestObject::GetTransformXM();
//}
