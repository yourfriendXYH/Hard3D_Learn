//#include "AssimpTest.h"
//#include "..\TransformCBuf.h"
//// 资源导入
//#include "../assimp/include/assimp/Importer.hpp"
//#include "../assimp/include/assimp/scene.h"
//#include "../assimp/include/assimp/postprocess.h"
//#include "..\VertexBuffer.h"
//#include "..\IndexBuffer.h"
//#include "..\VertexShader.h"
//#include "..\PixelShader.h"
//#include "..\InputLayout.h"
//#include "..\Topology.h"
//
//AssimpTest::AssimpTest(Graphics& gfx, std::mt19937& rng, 
//	std::uniform_real_distribution<float>& adist, 
//	std::uniform_real_distribution<float>& ddist, 
//	std::uniform_real_distribution<float>& odist, 
//	std::uniform_real_distribution<float>& rdist, 
//	DirectX::XMFLOAT3 material, float scale)
//	:
//	TestObject(gfx, rng, adist, ddist, odist, rdist)
//{
//	if (!IsStaticInitialized())
//	{
//		struct Vertex 
//		{
//			DirectX::XMFLOAT3 position;
//			DirectX::XMFLOAT3 normal;
//		};
//
//		Assimp::Importer assetImport;
//		const auto model = assetImport.ReadFile("models\\suzanne.obj",
//			aiProcess_Triangulate |
//			aiProcess_JoinIdenticalVertices);
//
//		// 获取模型的顶点数据
//		const auto pMesh = model->mMeshes[0];
//		std::vector<Vertex> vertices;
//		vertices.reserve(pMesh->mNumVertices);
//		for (unsigned int i = 0u; i < pMesh->mNumVertices; ++i)
//		{
//			vertices.push_back({ 
//				{pMesh->mVertices[i].x * scale, pMesh->mVertices[i].y * scale, pMesh->mVertices[i].z * scale},
//				*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i])
//				});
//		}
//
//		// 获取模型的索引数据
//		std::vector<unsigned short> indices;
//		indices.reserve(pMesh->mNumFaces * 3u);
//		for (unsigned int i = 0u; i < pMesh->mNumFaces; ++i)
//		{
//			const auto& face = pMesh->mFaces[i];
//			assert(face.mNumIndices == 3u);
//			indices.emplace_back(face.mIndices[0]);
//			indices.emplace_back(face.mIndices[1]);
//			indices.emplace_back(face.mIndices[2]);
//		}
//
//		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));
//		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
//
//		auto pvs = std::make_unique<VertexShader>(gfx, "PhongVS.cso");
//		auto pvsbc = pvs->GetByteCode();
//		AddStaticBind(std::move(pvs));
//
//		AddStaticBind(std::make_unique<PixelShader>(gfx, "PhongPS.cso"));
//
//		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
//		{
//			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
//			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
//		};
//		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
//
//		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//	}
//	else
//	{
//		SetStaticIndexBuffer();
//	}
//
//	struct PSMaterialConstant
//	{
//		DirectX::XMFLOAT3 color;
//		float specularIntensity = 0.6f;
//		float specularPower = 30.0f;
//		float padding[3];
//	} pmc;
//	pmc.color = material;
//	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
//
//	// 模型变换缓存
//	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
//}
