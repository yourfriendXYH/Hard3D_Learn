#pragma once

#include "../IndexedTriangleList.h"

class Cube
{
public:

	static IndexedTriangleList MakeIndependent(DynamicData::VertexLayout layout)
	{
		using namespace DynamicData;
		using ElemetType = DynamicData::VertexLayout::ElementType;

		constexpr float side = 1.0f / 2.0f;

		// 正方体的顶点数据 （不共用顶点）
		VerticesBuffer verticeBuf{ std::move(layout), 24u };
		verticeBuf[0].Attr<ElemetType::Position3D>() = { -side, -side, -side };
		verticeBuf[1].Attr<ElemetType::Position3D>() = { side,-side,-side };// 1
		verticeBuf[2].Attr<ElemetType::Position3D>() = { -side,side,-side };// 2
		verticeBuf[3].Attr<ElemetType::Position3D>() = { side,side,-side };// 3
		verticeBuf[4].Attr<ElemetType::Position3D>() = { -side,-side,side };// 4 far side
		verticeBuf[5].Attr<ElemetType::Position3D>() = { side,-side,side };// 5
		verticeBuf[6].Attr<ElemetType::Position3D>() = { -side,side,side };// 6
		verticeBuf[7].Attr<ElemetType::Position3D>() = { side,side,side };// 7
		verticeBuf[8].Attr<ElemetType::Position3D>() = { -side,-side,-side };// 8 left side
		verticeBuf[9].Attr<ElemetType::Position3D>() = { -side,side,-side };// 9
		verticeBuf[10].Attr<ElemetType::Position3D>() = { -side,-side,side };// 10
		verticeBuf[11].Attr<ElemetType::Position3D>() = { -side,side,side };// 11
		verticeBuf[12].Attr<ElemetType::Position3D>() = { side,-side,-side };// 12 right side
		verticeBuf[13].Attr<ElemetType::Position3D>() = { side,side,-side };// 13
		verticeBuf[14].Attr<ElemetType::Position3D>() = { side,-side,side };// 14
		verticeBuf[15].Attr<ElemetType::Position3D>() = { side,side,side };// 15
		verticeBuf[16].Attr<ElemetType::Position3D>() = { -side,-side,-side };// 16 bottom side
		verticeBuf[17].Attr<ElemetType::Position3D>() = { side,-side,-side };// 17
		verticeBuf[18].Attr<ElemetType::Position3D>() = { -side,-side,side };// 18
		verticeBuf[19].Attr<ElemetType::Position3D>() = { side,-side,side };// 19
		verticeBuf[20].Attr<ElemetType::Position3D>() = { -side,side,-side };// 20 top side
		verticeBuf[21].Attr<ElemetType::Position3D>() = { side,side,-side };// 21
		verticeBuf[22].Attr<ElemetType::Position3D>() = { -side,side,side };// 22
		verticeBuf[23].Attr<ElemetType::Position3D>() = { side,side,side };// 23

		// 返回顶点数据和索引数据
		return { std::move(verticeBuf), {
				0,2, 1,    2,3,1,
				4,5, 7,    4,7,6,
				8,10, 9,  10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23
			} };
	}

	// 生成纹理坐标数据
	static IndexedTriangleList MakeIndependentTextured()
	{
		using namespace DynamicData;
		using ElemetType = DynamicData::VertexLayout::ElementType;

		// 顶点包含 顶点位置、法线方向、纹理坐标的数据
		auto triangleList = MakeIndependent(VertexLayout{}.
			Append(ElemetType::Position3D).
			Append(ElemetType::Normal).
			Append(ElemetType::Texture2D));

		triangleList.m_vertices[0].Attr<ElemetType::Texture2D>() = { 0.0f, 0.0f };
		triangleList.m_vertices[1].Attr<ElemetType::Texture2D>() = { 1.0f,0.0f };
		triangleList.m_vertices[2].Attr<ElemetType::Texture2D>() = { 0.0f,1.0f };
		triangleList.m_vertices[3].Attr<ElemetType::Texture2D>() = { 1.0f,1.0f };
		triangleList.m_vertices[4].Attr<ElemetType::Texture2D>() = { 0.0f,0.0f };
		triangleList.m_vertices[5].Attr<ElemetType::Texture2D>() = { 1.0f,0.0f };
		triangleList.m_vertices[6].Attr<ElemetType::Texture2D>() = { 0.0f,1.0f };
		triangleList.m_vertices[7].Attr<ElemetType::Texture2D>() = { 1.0f,1.0f };
		triangleList.m_vertices[8].Attr<ElemetType::Texture2D>() = { 0.0f,0.0f };
		triangleList.m_vertices[9].Attr<ElemetType::Texture2D>() = { 1.0f,0.0f };
		triangleList.m_vertices[10].Attr<ElemetType::Texture2D>() = { 0.0f,1.0f };
		triangleList.m_vertices[11].Attr<ElemetType::Texture2D>() = { 1.0f,1.0f };
		triangleList.m_vertices[12].Attr<ElemetType::Texture2D>() = { 0.0f,0.0f };
		triangleList.m_vertices[13].Attr<ElemetType::Texture2D>() = { 1.0f,0.0f };
		triangleList.m_vertices[14].Attr<ElemetType::Texture2D>() = { 0.0f,1.0f };
		triangleList.m_vertices[15].Attr<ElemetType::Texture2D>() = { 1.0f,1.0f };
		triangleList.m_vertices[16].Attr<ElemetType::Texture2D>() = { 0.0f,0.0f };
		triangleList.m_vertices[17].Attr<ElemetType::Texture2D>() = { 1.0f,0.0f };
		triangleList.m_vertices[18].Attr<ElemetType::Texture2D>() = { 0.0f,1.0f };
		triangleList.m_vertices[19].Attr<ElemetType::Texture2D>() = { 1.0f,1.0f };
		triangleList.m_vertices[20].Attr<ElemetType::Texture2D>() = { 0.0f,0.0f };
		triangleList.m_vertices[21].Attr<ElemetType::Texture2D>() = { 1.0f,0.0f };
		triangleList.m_vertices[22].Attr<ElemetType::Texture2D>() = { 0.0f,1.0f };
		triangleList.m_vertices[23].Attr<ElemetType::Texture2D>() = { 1.0f,1.0f };

		return triangleList;
	}
};