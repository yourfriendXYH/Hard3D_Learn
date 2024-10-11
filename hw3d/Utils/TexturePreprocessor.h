#pragma once

#include "../Surface.h"
#include <string>
#include <DirectXMath.h>

class TexturePreprocessor
{
public:
	// 法线纹理Y值反向（处理多个纹理）
	static void FlipYAllNormalMapsInObj(const std::string& objPath);
	// 法线纹理Y值反向（处理单个纹理）
	static void FlipYNormalMap(const std::string& pathIn, const std::string& pathOut);
	// 
	static void ValidateNormalMap(const std::string& pathIn, float thresholdMin, float thresholdMax);

private:
	template<typename F>
	static void TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func);
	template<typename F>
	static void TransformSurface(Surface& surface, F&& func);

	static DirectX::XMVECTOR ColorToVector(Surface::Color color) noexcept;
	static Surface::Color VectorToColor(DirectX::FXMVECTOR normal) noexcept;
};
