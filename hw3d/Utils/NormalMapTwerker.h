#pragma once
#include <string>
#include <DirectXMath.h>
#include "../XYHMath.h"
#include "../Surface.h"

// 扭转法线贴图
class NormalMapTwerker
{
public:
	static void RotateXAxis180(const std::string& pathIn, const std::string& pathOut)
	{
		// 构造绕X轴的旋转矩阵
		const auto rotation = DirectX::XMMatrixRotationX(PI); // 旋转180
		auto surfaceIn = Surface::FromFile(pathIn);

		const auto numPixels = surfaceIn.GetWidth() * surfaceIn.GetHeight();
		const auto pBegin = surfaceIn.GetBufferPtr();
		const auto pEnd = surfaceIn.GetBufferPtrConst() + numPixels;

		for (auto pCurrent = pBegin; pCurrent < pEnd; ++pCurrent)
		{
			auto normal = ColorToVector(*pCurrent);
			normal = DirectX::XMVector3Transform(normal, rotation);
			*pCurrent = VectorToColor(normal);
		}

		surfaceIn.Save(pathOut);
	}

	static void RotateXAxis180(const std::string& pathIn)
	{
		return RotateXAxis180(pathIn, pathIn);
	}

private:
	static DirectX::XMVECTOR ColorToVector(Surface::Color color)
	{
		auto normal = DirectX::XMVectorSet((float)color.GetR(), (float)color.GetG(), (float)color.GetB(), 0.0f);
		const auto all255 = DirectX::XMVectorReplicate(2.0f / 255.0f);
		normal = DirectX::XMVectorMultiply(normal, all255);
		const auto all1 = DirectX::XMVectorReplicate(1.0f);
		normal = DirectX::XMVectorSubtract(normal, all1); // 减去矢量1
		return normal;
	}

	static Surface::Color VectorToColor(DirectX::XMVECTOR normalVector)
	{
		const auto all1 = DirectX::XMVectorReplicate(1.0f);
		DirectX::XMVECTOR normalOut = DirectX::XMVectorAdd(normalVector, all1);
		const auto all255 = DirectX::XMVectorReplicate(255.0f / 2.0f);
		normalOut = DirectX::XMVectorMultiply(normalOut, all255);

		DirectX::XMFLOAT3 floats;
		DirectX::XMStoreFloat3(&floats, normalOut);

		return { (unsigned char)round(floats.x), (unsigned char)round(floats.y), (unsigned char)round(floats.z) };
	}

};