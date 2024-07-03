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
	void SetNormalsIndependentFlat()
	{
		using namespace DirectX;
		using Type = DynamicData::VertexLayout::ElementType;
		for (size_t i = 0; i < m_indices.size(); i += 3)
		{
			auto v0 = m_vertices[m_indices[i]];
			auto v1 = m_vertices[m_indices[i + 1]];
			auto v2 = m_vertices[m_indices[i + 2]];
			const auto p0 = XMLoadFloat3(&v0.Attr<Type::Position3D>());
			const auto p1 = XMLoadFloat3(&v1.Attr<Type::Position3D>());
			const auto p2 = XMLoadFloat3(&v2.Attr<Type::Position3D>());

			const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

			XMStoreFloat3(&v0.Attr<Type::Normal>(), n);
			XMStoreFloat3(&v1.Attr<Type::Normal>(), n);
			XMStoreFloat3(&v2.Attr<Type::Normal>(), n);
		}
	}

public:
	DynamicData::VerticesBuffer m_vertices;
	std::vector<unsigned short> m_indices;
};