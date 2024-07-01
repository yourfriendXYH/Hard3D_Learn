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
//		// 顶点结构
//		struct Vertex
//		{
//			DirectX::XMFLOAT3 pos;
//			DirectX::XMFLOAT3 normal;
//		};
//
//		// 生成Cube的模型数据
//		auto model = Cube::MakeIndependent<Vertex>(); // 棱柱体
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
//		// 创建顶点缓存（输入装配阶段）
//		//AddBind(std::make_unique<VertexBuffer>(gfx, vertices));
//		//AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));
//		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.m_vertices));
//
//		// 创建顶点着色器
//		auto pVertexShader = std::make_unique<VertexShader>(gfx, "PhongVS.cso");
//		auto pVertexShaderBtyeCode = pVertexShader->GetByteCode();
//		// （顶点着色器阶段）
//		//AddBind(std::move(pVertexShader));
//		AddStaticBind(std::move(pVertexShader));
//		// 添加像素着色器（像素着色器阶段）
//		//AddBind(std::make_unique<PixelShader>(gfx, "PixelShader.cso"));
//		AddStaticBind(std::make_unique<PixelShader>(gfx, "PhongPS.cso"));
//
//		// 添加索引缓存（顶点着色器阶段）
//		// AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
//		//AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
//		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_indices));
//
//		// 添加输入布局（输入装配阶段）
//		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
//		{
//			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//			{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		};
//		//AddBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pVertexShaderBtyeCode)); // 需要传入顶点着色器的字节码
//		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pVertexShaderBtyeCode)); // 需要传入顶点着色器的字节码
//
//		// 添加基元拓扑（输入装配阶段）
//		//AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//	}
//	else
//	{
//		// 除第一个外，后面drawable的索引缓存需要用前面的赋值，索引缓存不能改为static，它会根据顶点缓存的数量变化
//		SetStaticIndexBuffer();
//	}
//
//	// 添加初始的变换（顶点着色器阶段）
//	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
//
//	// Cube颜色的常量缓存
//	struct PSMaterialConstant
//	{
//		alignas(16) DirectX::XMFLOAT3 m_color; // 16字节对齐
//		float specularIntensity = 0.6f; // 镜面高光强度
//		float specularPower = 30.f; // 镜面高光功率
//		float m_padding[2]; // 对齐留空
//	} colorConstant;
//	colorConstant.m_color = material;
//	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConstant, 1u));
//
//	// 自生的缩放矩阵
//	DirectX::XMStoreFloat3x3(&m_mt, DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f));
//}
//
//DirectX::XMMATRIX TestBox::GetTransformXM() const noexcept
//{
//	return DirectX::XMLoadFloat3x3(&m_mt) * TestObject::GetTransformXM();
//}
