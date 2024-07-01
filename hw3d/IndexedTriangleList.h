#pragma once
#include <vector>
#include "DynamicData/DynamicVertex.h"

// 三角面列表

class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(DynamicData::VerticesBuffer vertices_in, std::vector<unsigned short> indices_in)
		:
		m_vertices(vertices_in),
		m_indices(indices_in)
	{
		assert(m_vertices.VerticesSize() > 2); //三个点才能确定一个面
		assert(m_indices.size() % 3 == 0); // 索引数要是3的倍数
	}
	// 顶点变换
	void Transform(DirectX::FXMMATRIX matrix)
	{
		using ElementType = DynamicData::VertexLayout::ElementType;

		auto tempSize = m_vertices.VerticesSize();

		for (int i = 0; i < m_vertices.VerticesSize(); ++i)
		{
			auto& pos = m_vertices[i].Attr<ElementType::Position3D>();
			DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix));
		}
	}

	// 生成每个顶点的法线方向
	//void SetNormalsIndependentFlat()
	//{
	//	using namespace DirectX;
	//	assert(m_indices.size() % 3 == 0 && m_indices.size() > 0);
	//	for (size_t i = 0; i < m_indices.size(); i += 3)
	//	{
	//		auto& vertex0 = m_vertices[m_indices[i]];
	//		auto& vertex1 = m_vertices[m_indices[i + 1]];
	//		auto& vertex2 = m_vertices[m_indices[i + 2]];

	//		DirectX::XMVECTOR pos0 = DirectX::XMLoadFloat3(&vertex0.pos);
	//		DirectX::XMVECTOR pos1 = DirectX::XMLoadFloat3(&vertex1.pos);
	//		DirectX::XMVECTOR pos2 = DirectX::XMLoadFloat3(&vertex2.pos);

	//		// 叉乘算法线方向
	//		DirectX::XMVECTOR vector1 = pos1 - pos0;
	//		DirectX::XMVECTOR vector2 = pos2 - pos0;
	//		auto normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vector1, vector2));
	//		// 存储法线方向
	//		DirectX::XMStoreFloat3(&vertex0.normal, normal);
	//		DirectX::XMStoreFloat3(&vertex1.normal, normal);
	//		DirectX::XMStoreFloat3(&vertex2.normal, normal);
	//	}
	//}

public:
	DynamicData::VerticesBuffer m_vertices;
	std::vector<unsigned short> m_indices;
};