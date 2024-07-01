//#include "Cylinder.h"
//#include "..\Prism.h"
//#include "..\VertexBuffer.h"
//
//Cylinder::Cylinder(Graphics& gfx, std::mt19937& rng, 
//	std::uniform_real_distribution<float>& adist, 
//	std::uniform_real_distribution<float>& ddist, 
//	std::uniform_real_distribution<float>& odist, 
//	std::uniform_real_distribution<float>& rdist, 
//	std::uniform_real_distribution<float>& bdist, 
//	std::uniform_int_distribution<int>& tdist)
//	:
//	TestObject(gfx, rng, adist, ddist, odist, rdist)
//{
//	if (!IsStaticInitialized())
//	{
//		struct Vertex 
//		{
//			DirectX::XMFLOAT3 pos;
//			DirectX::XMFLOAT3 n;
//		};
//
//		auto model = Prism::MakeTesselatedIndependentCapNormals<Vertex>(tdist(rng));
//		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.m_vertices));
//
//	}
//	else
//	{
//
//	}
//}
