#pragma once
#include "IndexedTriangleList.h"

class Cube
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeSkinned()
	{
		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(14);

		vertices[0].pos = { -side,-side,-side };
		vertices[0].tex = { 2.0f / 3.0f,0.0f / 4.0f };
		vertices[1].pos = { side,-side,-side };
		vertices[1].tex = { 1.0f / 3.0f,0.0f / 4.0f };
		vertices[2].pos = { -side,side,-side };
		vertices[2].tex = { 2.0f / 3.0f,1.0f / 4.0f };
		vertices[3].pos = { side,side,-side };
		vertices[3].tex = { 1.0f / 3.0f,1.0f / 4.0f };
		vertices[4].pos = { -side,-side,side };
		vertices[4].tex = { 2.0f / 3.0f,3.0f / 4.0f };
		vertices[5].pos = { side,-side,side };
		vertices[5].tex = { 1.0f / 3.0f,3.0f / 4.0f };
		vertices[6].pos = { -side,side,side };
		vertices[6].tex = { 2.0f / 3.0f,2.0f / 4.0f };
		vertices[7].pos = { side,side,side };
		vertices[7].tex = { 1.0f / 3.0f,2.0f / 4.0f };
		vertices[8].pos = { -side,-side,-side };
		vertices[8].tex = { 2.0f / 3.0f,4.0f / 4.0f };
		vertices[9].pos = { side,-side,-side };
		vertices[9].tex = { 1.0f / 3.0f,4.0f / 4.0f };
		vertices[10].pos = { -side,-side,-side };
		vertices[10].tex = { 3.0f / 3.0f,1.0f / 4.0f };
		vertices[11].pos = { -side,-side,side };
		vertices[11].tex = { 3.0f / 3.0f,2.0f / 4.0f };
		vertices[12].pos = { side,-side,-side };
		vertices[12].tex = { 0.0f / 3.0f,1.0f / 4.0f };
		vertices[13].pos = { side,-side,side };
		vertices[13].tex = { 0.0f / 3.0f,2.0f / 4.0f };

		return{
			std::move(vertices),{
				0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
			}
		};
	}

	// 绘制相邻面顶点不共用的立方体，宽度为1
	template<class V>
	static IndexedTriangleList<V> MakeIndependent()
	{
		constexpr float side = 1.0f / 2.0f;
		std::vector<V> vertices(24); // 每个面四个点

		// 模型顶点坐标，不共用顶点
		// near face
		vertices[0].pos = { -side, -side, -side };
		vertices[1].pos = { side, -side, -side };
		vertices[2].pos = { -side, side, -side };
		vertices[3].pos = { side, side, -side };
		// far face
		vertices[4].pos = { -side,-side,side };
		vertices[5].pos = { side,-side,side };
		vertices[6].pos = { -side,side,side };
		vertices[7].pos = { side,side,side };
		// left face
		vertices[8].pos = { -side,-side,-side };
		vertices[9].pos = { -side,side,-side };
		vertices[10].pos = { -side,-side,side };
		vertices[11].pos = { -side,side,side };
		// right face
		vertices[12].pos = { side,-side,-side };
		vertices[13].pos = { side,side,-side };
		vertices[14].pos = { side,-side,side };
		vertices[15].pos = { side,side,side };
		// bottom face
		vertices[16].pos = { -side,-side,-side };
		vertices[17].pos = { side,-side,-side };
		vertices[18].pos = { -side,-side,side };
		vertices[19].pos = { side,-side,side };
		// top face
		vertices[20].pos = { -side,side,-side };
		vertices[21].pos = { side,side,-side };
		vertices[22].pos = { -side,side,side };
		vertices[23].pos = { side,side,side };

		return { std::move(vertices), // 顶点坐标数据
				{0,2,1, 2,3,1,		// 顶点索引数据
				4,5,7, 4,7,6,
				8,10,9, 10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23}
		};
	}
};