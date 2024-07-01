#pragma once
#include <DirectXMath.h>
#include "IndexedTriangleList.h"
#include "XYHMath.h"

// ������
class Prism
{
public:
	// ���ö���
	template<class V>
	static IndexedTriangleList<V> MakeTesselate(int longDiv)
	{
		assert(longDiv >= 3);
		const auto base = DirectX::XMVectorSet(1.f, 0.f, -1.f, 0.f);
		const auto offset = DirectX::XMVectorSet(0.f, 0.f, 2.f, 0.f);
		const float longitudeAngle = 2 * PI / longDiv;

		std::vector<V> vertices;
		// �ײ���
		vertices.emplace_back();
		vertices.back().pos = { 0.f, 0.f, -1.f };
		const auto indexCenterNear = (unsigned short)(vertices.size() - 1); // 0
		// ������
		vertices.emplace_back();
		vertices.back().pos = { 0.f, 0.f, 1.f };
		const auto indexCenterFar = (unsigned short)(vertices.size() - 1); // 1

		// �����Ķ�������
		for (int indexLong = 0; indexLong < longDiv; ++indexLong)
		{
			// near base
			{
				vertices.emplace_back();
				// ��Z����ת
				auto vertexNear = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(longitudeAngle * indexLong));
				DirectX::XMStoreFloat3(&vertices.back().pos, vertexNear);
			}

			// far base
			{
				vertices.emplace_back();
				// ��Z����ת
				auto vertexFar = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(longitudeAngle * indexLong));
				// Z��ƫ��2
				vertexFar = DirectX::XMVectorAdd(vertexFar, offset);
				DirectX::XMStoreFloat3(&vertices.back().pos, vertexFar);
			}
		}

		// ��������������
		// side indices
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			indices.push_back(i + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(i + 1 + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back((i + 3) % mod + 2);
			indices.push_back(i + 1 + 2);
		}

		// base indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			indices.push_back(i + 2);
			indices.push_back(indexCenterNear);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(indexCenterFar);
			indices.push_back(i + 1 + 2);
			indices.push_back((i + 3) % mod + 2);
		}

		return { std::move(vertices),std::move(indices) };

	}

	// �����ö��㣬ͬʱ���㶥��ķ��߷���
	template<class V>
	static IndexedTriangleList<V> MakeTesselatedIndependentCapNormals(int longDiv)
	{
		namespace dx = DirectX;
		assert(longDiv >= 3);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const auto offset = dx::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / longDiv;

		std::vector<V> vertices;

		// near center
		const auto iCenterNear = (unsigned short)vertices.size();
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		vertices.back().n = { 0.0f,0.0f,-1.0f };
		// near base vertices
		const auto iBaseNear = (unsigned short)vertices.size();
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			auto v = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			dx::XMStoreFloat3(&vertices.back().pos, v);
			vertices.back().n = { 0.0f,0.0f,-1.0f };
		}
		// far center
		const auto iCenterFar = (unsigned short)vertices.size();
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		vertices.back().n = { 0.0f,0.0f,1.0f };
		// far base vertices
		const auto iBaseFar = (unsigned short)vertices.size();
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			auto v = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			v = dx::XMVectorAdd(v, offset);
			dx::XMStoreFloat3(&vertices.back().pos, v);
			vertices.back().n = { 0.0f,0.0f,1.0f };
		}
		// fusilage vertices
		const auto iFusilage = (unsigned short)vertices.size();
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			// near base
			{
				vertices.emplace_back();
				auto v = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				dx::XMStoreFloat3(&vertices.back().pos, v);
				vertices.back().n = { vertices.back().pos.x,vertices.back().pos.y,0.0f };
			}
			// far base
			{
				vertices.emplace_back();
				auto v = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				v = dx::XMVectorAdd(v, offset);
				dx::XMStoreFloat3(&vertices.back().pos, v);
				// ���߷���
				vertices.back().n = { vertices.back().pos.x,vertices.back().pos.y,0.0f };
			}
		}

		std::vector<unsigned short> indices;

		// near base indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			// near
			indices.push_back(i + iBaseNear);
			indices.push_back(iCenterNear);
			indices.push_back((i + 2) % mod + iBaseNear);
		}
		// far base indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			// far
			indices.push_back(iCenterFar);
			indices.push_back(i + 1 + iBaseFar);
			indices.push_back((i + 3) % mod + iBaseFar);
		}
		// fusilage indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			indices.push_back(i + iFusilage);
			indices.push_back((i + 2) % mod + iFusilage);
			indices.push_back(i + 1 + iFusilage);
			indices.push_back((i + 2) % mod + iFusilage);
			indices.push_back((i + 3) % mod + iFusilage);
			indices.push_back(i + 1 + iFusilage);
		}

		return { std::move(vertices),std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselate<V>(24);
	}
};

