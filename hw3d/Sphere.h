#pragma once
#include "IndexedTriangleList.h"
#include "XYHMath.h"
#include <DirectXMath.h>
#include <optional>
#include "DynamicData/DynamicVertex.h"



// ����
class Sphere
{
public:
	// �������徭γ�ߵ���������������Ķ������ݺͶ�����������
	static IndexedTriangleList MakeTesselated(DynamicData::VertexLayout vertexLayout, int latDiv, int longDiv)
	{
		assert(latDiv >= 3);
		assert(longDiv >= 3);

		// �������������ϵ
		constexpr float radius = 1.0f;
		const auto base = DirectX::XMVectorSet(0.f, 0.f, radius, 0.f);
		const float latitudeAngle = PI / latDiv; // γ������Ļ���
		const float longitudeAngle = 2 * PI / longDiv; // ��������Ļ���

		// �������嶥�����ݣ�pos��XMFloat3����
		DynamicData::VerticesBuffer verticesBuffer{ std::move(vertexLayout) }; //��̬��������
		for (int indexLat = 1; indexLat < latDiv; ++indexLat)
		{
			// ��X����ת������γ�ߵ���ת�㣬DirectXΪ ��������ϵ
			const auto latBase = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationX(latitudeAngle * indexLat)); // ˳ʱ����ת
			for (int indexLong = 0; indexLong < longDiv; ++indexLong)
			{
				DirectX::XMFLOAT3 calculatedPos;
				auto sphereVertex = DirectX::XMVector3Transform(latBase, DirectX::XMMatrixRotationZ(longitudeAngle * indexLong));
				// ���Ķ����������һ��ֵ����emplace_back��Ӧ
				DirectX::XMStoreFloat3(&calculatedPos, sphereVertex);
				// ����������������̬����
				verticesBuffer.EmplaceBack(calculatedPos);
			}
		}
		// ������
		const auto indexNorthPole = (unsigned int)verticesBuffer.Size();
		{
			DirectX::XMFLOAT3 northPos;
			DirectX::XMStoreFloat3(&northPos, base);
			verticesBuffer.EmplaceBack(northPos);
		}
		
		// �ϼ���
		const auto indexSouthPole = (unsigned int)verticesBuffer.Size();
		{
			DirectX::XMFLOAT3 southPos;
			DirectX::XMStoreFloat3(&southPos, DirectX::XMVectorNegate(base)); // ������
			verticesBuffer.EmplaceBack(southPos);
		}

		// ��������
		const auto calcIdx = [latDiv, longDiv](unsigned int indexLat, unsigned int indexLong)
		{
			return indexLat * longDiv + indexLong;
		};
		// ���巽����Ƭ����������
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
			// ���һ����Ƭ
			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, 0));
		}

		// �ϱ��������������������Ƭ
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
		// ���һ����������Ƭ
		// north
		indices.push_back(indexNorthPole);
		indices.push_back(calcIdx(0, longDiv - 1));
		indices.push_back(calcIdx(0, 0));
		// south
		indices.push_back(calcIdx(latDiv - 2, 0));
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
		indices.push_back(indexSouthPole);

		// ���ƶ����壬�����ƶ����죬���ٶ������ݺ��������ݵĿ���
		return { std::move(verticesBuffer), std::move(indices) };
	}

	static IndexedTriangleList Make(std::optional<DynamicData::VertexLayout> vertexLayout = std::nullopt)
	{
		using Element = DynamicData::VertexLayout::ElementType;
		if (!vertexLayout)
		{
			vertexLayout = DynamicData::VertexLayout{}.Append(Element::Position3D);
		}

		// γ�� 11�� ���� 24
		return MakeTesselated(std::move(*vertexLayout),12, 24);
	}
};