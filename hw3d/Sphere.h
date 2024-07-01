#pragma once
#include "IndexedTriangleList.h"
#include "XYHMath.h"
#include <DirectXMath.h>
#include <optional>
#include "DynamicData/DynamicVertex.h"



// 球体
class Sphere
{
public:
	// 传入球体经纬线的数量，生成球体的顶点数据和顶点索引数据
	static IndexedTriangleList MakeTesselated(DynamicData::VertexLayout vertexLayout, int latDiv, int longDiv)
	{
		assert(latDiv >= 3);
		assert(longDiv >= 3);

		// 球体自身的坐标系
		constexpr float radius = 1.0f;
		const auto base = DirectX::XMVectorSet(0.f, 0.f, radius, 0.f);
		const float latitudeAngle = PI / latDiv; // 纬线相隔的弧度
		const float longitudeAngle = 2 * PI / longDiv; // 经线相隔的弧度

		// 计算球体顶点数据，pos是XMFloat3类型
		DynamicData::VerticesBuffer verticesBuffer{ std::move(vertexLayout) }; //动态顶点数据
		for (int indexLat = 1; indexLat < latDiv; ++indexLat)
		{
			// 绕X轴旋转，生成纬线的旋转点，DirectX为 左手坐标系
			const auto latBase = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationX(latitudeAngle * indexLat)); // 顺时针旋转
			for (int indexLong = 0; indexLong < longDiv; ++indexLong)
			{
				DirectX::XMFLOAT3 calculatedPos;
				auto sphereVertex = DirectX::XMVector3Transform(latBase, DirectX::XMMatrixRotationZ(longitudeAngle * indexLong));
				// 更改顶点数组最后一个值，与emplace_back对应
				DirectX::XMStoreFloat3(&calculatedPos, sphereVertex);
				// 将顶点数据塞给动态顶点
				verticesBuffer.EmplaceBack(calculatedPos);
			}
		}
		// 北极点
		const auto indexNorthPole = (unsigned int)verticesBuffer.Size();
		{
			DirectX::XMFLOAT3 northPos;
			DirectX::XMStoreFloat3(&northPos, base);
			verticesBuffer.EmplaceBack(northPos);
		}
		
		// 南极点
		const auto indexSouthPole = (unsigned int)verticesBuffer.Size();
		{
			DirectX::XMFLOAT3 southPos;
			DirectX::XMStoreFloat3(&southPos, DirectX::XMVectorNegate(base)); // 向量求反
			verticesBuffer.EmplaceBack(southPos);
		}

		// 计算索引
		const auto calcIdx = [latDiv, longDiv](unsigned int indexLat, unsigned int indexLong)
		{
			return indexLat * longDiv + indexLong;
		};
		// 球体方形面片的索引数据
		std::vector<unsigned short> indices;
		for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
		{
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				indices.push_back(calcIdx(iLat, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong + 1));
			}
			// 最后一个面片
			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, 0));
		}

		// 南北极点相关联的三角形面片
		for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
		{
			// north
			indices.push_back(indexNorthPole);
			indices.push_back(calcIdx(0, iLong));
			indices.push_back(calcIdx(0, iLong + 1));
			// south
			indices.push_back(calcIdx(latDiv - 2, iLong + 1));
			indices.push_back(calcIdx(latDiv - 2, iLong));
			indices.push_back(indexSouthPole);
		}
		// 最后一个三角形面片
		// north
		indices.push_back(indexNorthPole);
		indices.push_back(calcIdx(0, longDiv - 1));
		indices.push_back(calcIdx(0, 0));
		// south
		indices.push_back(calcIdx(latDiv - 2, 0));
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
		indices.push_back(indexSouthPole);

		// 用移动语义，调用移动构造，减少顶点数据和索引数据的拷贝
		return { std::move(verticesBuffer), std::move(indices) };
	}

	static IndexedTriangleList Make(std::optional<DynamicData::VertexLayout> vertexLayout = std::nullopt)
	{
		using Element = DynamicData::VertexLayout::ElementType;
		if (!vertexLayout)
		{
			vertexLayout = DynamicData::VertexLayout{}.Append(Element::Position3D);
		}

		// 纬线 11， 经线 24
		return MakeTesselated(std::move(*vertexLayout),12, 24);
	}
};