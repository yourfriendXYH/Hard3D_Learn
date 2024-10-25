#pragma once

#include "../Surface.h"
#include <string>
#include <DirectXMath.h>

class TexturePreprocessor
{
public:
	// ��������Yֵ���򣨴���������
	static void FlipYAllNormalMapsInObj(const std::string& objPath);
	// ��������Yֵ���򣨴���������
	static void FlipYNormalMap(const std::string& pathIn, const std::string& pathOut);
	// ��֤������ͼ��Ч��
	static void ValidateNormalMap(const std::string& pathIn, float thresholdMin, float thresholdMax);

	static void MakeStripes(const std::string& pathOut, int size, int stripeWidth);

private:
	template<typename F>
	static void TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func);
	template<typename F>
	static void TransformSurface(Surface& surface, F&& func);

	static DirectX::XMVECTOR ColorToVector(Surface::Color color) noexcept;
	static Surface::Color VectorToColor(DirectX::FXMVECTOR normal) noexcept;
};
